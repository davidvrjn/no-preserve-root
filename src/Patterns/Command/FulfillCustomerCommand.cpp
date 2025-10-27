#include "../../../include/Patterns/Command/FulfillCustomerCommand.h"

#include <utility>
#include <random>

#include "../../../include/Patterns/Builder/PlantSpecification.h"
#include "../../../include/Core/Inventory.h"
#include "../../../include/Actors/Customer.h"
#include "../../../include/Patterns/Iterator/Iterator.h"
#include "../../../include/Components/InventoryComponent.h"
#include "../../../include/Components/Plant.h"

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

    auto it = inv->createIterator();

    // Gather all plants from inventory
    std::vector<std::shared_ptr<Plant>> plants;
    while (it->hasNext()) {
        auto comp = it->next();
        auto plant = std::dynamic_pointer_cast<Plant>(comp);
        if (plant) {
            plants.push_back(plant);
        }
    }

    if (spec->requestType == RequestType::RECOMMENDATION) {
        // Shuffle and sample ~50% of the plants
        std::random_device rd;
        std::mt19937 gen(rd());
        std::shuffle(plants.begin(), plants.end(), gen);

        size_t sampleSize = plants.size() / 2;
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
        for (auto& plant : plants) {
            if (plant->getName() == spec->explicitName) {
                // Apply decorators in order
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

                targetId = plant->getId(); // keep original ID
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
