#include "../../../include/Patterns/Command/FulfillCustomerCommand.h"

#include <utility>
#include <algorithm>
#include <random>

#include "../../../include/Patterns/Builder/PlantSpecification.h"
#include "../../../include/Core/Inventory.h"
#include "../../../include/Actors/Customer.h"
#include "../../../include/Patterns/Iterator/Iterator.h"
#include "../../../include/Components/InventoryComponent.h"
#include "../../../include/Components/Plant.h"
#include "../../../include/Components/Group.h"

// Decorators
#include "../../../include/Patterns/Decorator/GiftWrapDecorator.h"
#include "../../../include/Patterns/Decorator/PotDecorator.h"
#include "../../../include/Patterns/Decorator/RibbonDecorator.h"



FulfillCustomerCommand::FulfillCustomerCommand(std::unique_ptr<PlantSpecification> spec,
                                               const std::shared_ptr<Inventory>& inventory,
                                               const std::shared_ptr<Customer>& customer) 
                    : spec(std::move(spec)),
                    inventory(inventory),
                    customer(customer),
                    status(Status::Pending),
                    targetId(0) {}

void FulfillCustomerCommand::execute() {
    auto inv = inventory.lock();
    auto cust = customer.lock();

    if (!inv || !cust || !spec) {
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
                targetId = plant->getId();
                status = Status::Completed;
                return;
            }
        }
        status = Status::Failed;
    } 
    else if (spec->requestType == RequestType::PURCHASE) {
        // Only search in "Storage"
        auto storage = inv->findGroupByName("Storage");
        if (!storage) {
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
                salePrice = decorated->getPrice();
                targetId = plant->getId();

                // Remove from storage
                storage->remove(plant);

                status = Status::Completed;
                return;
            }
        }
        status = Status::Failed;
    } 
    else {
        status = Status::Failed;
    }
}
   

std::string FulfillCustomerCommand::serialize() const { return std::string(); }
void FulfillCustomerCommand::deserialize(const std::string& data) { (void)data; }
FulfillCustomerCommand::Status FulfillCustomerCommand::getStatus() const { return status; }
void FulfillCustomerCommand::setStatus(Status s) { status = s; }
uint64_t FulfillCustomerCommand::getTargetId() const { return targetId;}
void FulfillCustomerCommand::setTargetId(uint64_t id) { targetId = id; }
