#include "../../../include/Patterns/Command/FulfillCustomerCommand.h"

#include <algorithm>
#include <fstream>
#include <random>
#include <utility>
#include <sstream>

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

FulfillCustomerCommand::FulfillCustomerCommand(std::unique_ptr<PlantSpecification> spec,
                                               const std::shared_ptr<Inventory>& inventory,
                                               const std::shared_ptr<Nursery>& nursery)
    : spec(std::move(spec)),
      inventory(inventory),
      nursery(nursery),
      status(Status::Pending),
      targetId(0) {}

void FulfillCustomerCommand::execute() {
    auto inv = inventory.lock();
    auto nur = nursery.lock();

    if (!inv || !nur || !spec) {
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

            if (plant->typeName() == spec->explicitName) {
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

FulfillCustomerCommand::Status FulfillCustomerCommand::getStatus() const { return status; }
void FulfillCustomerCommand::setStatus(Status s) { status = s; }
uint64_t FulfillCustomerCommand::getTargetId() const { return targetId; }
void FulfillCustomerCommand::setTargetId(uint64_t id) { targetId = id; }

std::string FulfillCustomerCommand::toString() const {
    std::ostringstream out;
    // Handle PURCHASE requests
    if (!spec) {
        return std::string("FulfillCustomerCommand");
    }

    if (spec->requestType == RequestType::PURCHASE) {
        const std::string& plantName = spec->explicitName.empty() ? std::string("<unknown>") : spec->explicitName;
        if (status == Status::Completed) {
            out << "Sold a " << plantName;
            if (!spec->decorators.empty()) {
                out << " with ";
                for (size_t i = 0; i < spec->decorators.size(); ++i) {
                    if (i) out << ",";
                    out << spec->decorators[i];
                }
            out << " for R" << salePrice;
            }
            return out.str();
        }
        else if (status == Status::Failed) {
            out << "Did not have " << plantName << " in stock";
            return out.str();
        }
        else {
            out << "Purchase: " << plantName;
            if (!spec->decorators.empty()) {
                out << " (decorators: ";
                for (size_t i = 0; i < spec->decorators.size(); ++i) {
                    if (i) out << ",";
                    out << spec->decorators[i];
                }
                out << ")";
            }
            return out.str();
        }
    }
    else { // RECOMMENDATION
        if (status == Status::Completed) {
            // Try to resolve plant name from inventory via targetId if possible
            auto inv = inventory.lock();
            if (inv) {
                auto plants = inv->getAllPlants();
                for (const auto &p : plants) {
                    if (p && p->getId() == targetId) {
                        out << "Recommended a " << p->getName();
                        return out.str();
                    }
                }
            }
            // Fallback
            out << "Recommended a plant";
            return out.str();
        }
        else if (status == Status::Failed) {
            out << "Could not recommend a plant";
            return out.str();
        }
        else {
            out << "Recommendation request";
            return out.str();
        }
    }
}
