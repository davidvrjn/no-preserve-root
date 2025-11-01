#include "../../../include/Patterns/Command/FulfillCustomerCommand.h"

#include <algorithm>
#include <random>
#include <utility>

#include "../../../include/Actors/Customer.h"
#include "../../../include/Components/Group.h"
#include "../../../include/Components/InventoryComponent.h"
#include "../../../include/Components/Plant.h"
#include "../../../include/Core/Inventory.h"
#include "../../../include/Core/Nursery.h"
#include "../../../include/Patterns/Builder/PlantSpecification.h"
#include "../../../include/Patterns/Iterator/Iterator.h"

// Decorators
#include "../../../include/Patterns/Decorator/GiftWrapDecorator.h"
#include "../../../include/Patterns/Decorator/PotDecorator.h"
#include "../../../include/Patterns/Decorator/RibbonDecorator.h"

/**
 * @brief Construct a new Fulfill Customer Command:: Fulfill Customer Command object
 * 
 * @param spec Unique pointer to the plant specification containing customer requirements.
 * @param inventory Shared pointer to the nursery's inventory.
 * @param customer Shared pointer to the customer making the request.
 * @param nursery Shared pointer to the nursery processing the request
 * 
 * The constructor initializes the command with Pending status and stores weak
 * references to prevent circular dependencies
 */
FulfillCustomerCommand::FulfillCustomerCommand(std::unique_ptr<PlantSpecification> spec,
                                               const std::shared_ptr<Inventory>& inventory,
                                               const std::shared_ptr<Customer>& customer,
                                               const std::shared_ptr<Nursery>& nursery)
    : spec(std::move(spec)),
      inventory(inventory),
      customer(customer),
      nursery(nursery),
      status(Status::Pending),
      targetId(0) {}

/**
 * @brief Executes the customer fulfillment command.
 * 
 * This method processes customer requests based on the request type:
 * 
 * **RECOMMENDATION requests:**
 * - Searches all plants in inventory
 * - Randomly samples ~50% of available plants
 * - Finds plants matching water and season requirements
 * - On success: Sets targetId, increases reputation by +3
 * - On failure: Decreases reputation by -5
 * 
 * **PURCHASE requests:**
 * - Searches only in the "Storage" group
 * - Finds plant by explicit name
 * - Applies requested decorators (GiftWrap, Pot, Ribbon)
 * - Calculates seasonal pricing:
 *   - Base plant price adjusted for current season
 *   - Decorator costs with seasonal multiplier (Winter: +20%)
 * - Removes plant from storage
 * - Adds sale revenue to nursery
 * - On success: Sets targetId, decoratedPlant, salePrice
 * - On failure: Decreases reputation by -5
 * 
 * The command fails if:
 * - Any weak_ptr (inventory, customer, nursery) has expired
 * - Specification is null
 * - No plants available (RECOMMENDATION)
 * - No matching plant found
 * - Storage group doesn't exist (PURCHASE)
 * 
 * @note Seasonal pricing applies Winter multiplier (1.2x) to decorators only
 */
void FulfillCustomerCommand::execute() {
    auto inv = inventory.lock();
    auto cust = customer.lock();
    auto nur = nursery.lock();

    if (!inv || !cust || !nur || !spec) {
        status = Status::Failed;
        return;
    }

    if (spec->requestType == RequestType::RECOMMENDATION) {
        // Gather all plants
        std::vector<std::shared_ptr<Plant>> plants;
        auto it = inv->createIterator();
        while (it->hasNext()) {
            auto comp = it->next();
            auto plant = std::dynamic_pointer_cast<Plant>(comp);
            if (plant) plants.push_back(plant);
        }

        if (plants.empty()) {
            // Failed: No plants to recommend
            nur->adjustReputation(-5);
            status = Status::Failed;
            return;
        }

        // Shuffle and take ~50%
        std::random_device rd;
        std::mt19937 gen(rd());
        std::shuffle(plants.begin(), plants.end(), gen);

        size_t sampleSize = std::max<size_t>(1, plants.size() / 2);
        for (size_t i = 0; i < sampleSize; ++i) {
            auto& plant = plants[i];
            if (plant->getWaterRequirement() == spec->waterReq &&
                plant->isSuitableForSeason(spec->seasonReq)) {
                // Success: Found matching plant
                targetId = plant->getId();
                nur->adjustReputation(+3);  // Good recommendation
                status = Status::Completed;
                return;
            }
        }

        // Failed: No matching plant found
        nur->adjustReputation(-5);
        status = Status::Failed;

    } else if (spec->requestType == RequestType::PURCHASE) {
        // Only search in "Storage"
        auto storage = inv->findGroupByName("Storage");
        if (!storage) {
            // Failed: No storage
            nur->adjustReputation(-5);
            status = Status::Failed;
            return;
        }

        auto it = storage->createIterator();
        while (it->hasNext()) {
            auto comp = it->next();
            auto plant = std::dynamic_pointer_cast<Plant>(comp);
            if (!plant) continue;

            if (plant->getName() == spec->explicitName) {
                // Apply decorators
                std::shared_ptr<InventoryComponent> decorated = plant;
                for (const auto& deco : spec->decorators) {
                    if (deco == "GiftWrap") {
                        decorated = std::make_shared<GiftWrapDecorator>(decorated);
                    } else if (deco == "Pot") {
                        decorated = std::make_shared<PotDecorator>(decorated);
                    } else if (deco == "Ribbon") {
                        decorated = std::make_shared<RibbonDecorator>(decorated);
                    }
                }

                // Store results
                decoratedPlant = decorated;
                targetId = plant->getId();

                // Apply seasonal pricing adjustment
                Season currentSeason = nur->getCurrentSeason();

                // Calculate seasonal plant price
                double basePlantPrice = plant->getSeasonalPrice(currentSeason);

                // Calculate decorator costs (difference between decorated and base)
                double baseDecoratorCosts = decorated->getPrice() - plant->getPrice();

                // Apply seasonal multiplier to decorators
                // Winter (lets say it Christmas season why dont we): +20% on decorators
                // Other seasons: No change
                double decoratorMultiplier = (currentSeason == Season::WINTER) ? 1.2 : 1.0;
                double seasonalDecoratorCosts = baseDecoratorCosts * decoratorMultiplier;

                // Final sale price = seasonal plant price + seasonal decorator costs
                salePrice = basePlantPrice + seasonalDecoratorCosts;

                // Remove from storage (Group::remove() will handle observer detachment)
                storage->remove(plant);

                // Success: Made a sale
                nur->adjustMoney(salePrice);  // Add revenue (no reputation change)
                status = Status::Completed;
                return;
            }
        }

        // Failed: Plant not found
        nur->adjustReputation(-5);
        status = Status::Failed;

    } else {
        status = Status::Failed;
    }
}

/**
 * @brief Gets the current status of the fulfillment command.
 * 
 * @return Status The current status (Pending, Completed, or Failed).
 */
FulfillCustomerCommand::Status FulfillCustomerCommand::getStatus() const { return status; }

/**
 * @brief Sets the status of the fulfillment command.
 * 
 * @param s The new status to set.
 */
void FulfillCustomerCommand::setStatus(Status s) { status = s; }

/**
 * @brief  Gets the ID of the target plant involved in this transaction.
 * 
 * @return uint64_t The unique identifier of the plant (0 if no plant was found).
 */
uint64_t FulfillCustomerCommand::getTargetId() const { return targetId; }

/**
 * @brief Sets the target plant ID for this command.
 * 
 * @param id The unique identifier of the plant.
 */
void FulfillCustomerCommand::setTargetId(uint64_t id) { targetId = id; }
