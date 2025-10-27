#include "../../../include/Patterns/Command/FulfillCustomerCommand.h"

#include <utility>

#include "../../../include/Patterns/Builder/PlantSpecification.h"
#include "../../../include/Core/Inventory.h"
#include "../../../include/Actors/Customer.h"
#include "../../../include/Patterns/Iterator/Iterator.h"
#include "../../../include/Components/InventoryComponent.h"

FulfillCustomerCommand::FulfillCustomerCommand(std::unique_ptr<PlantSpecification> spec,
                                               const std::shared_ptr<Inventory>& inventory,
                                               const std::shared_ptr<Customer>& customer) 
                    : spec(std::move(spec)),
                    inventory(inventory),
                    customer(customer),
                    status(Status::Pending),
                    targetId(0) {}

void FulfillCustomerCommand::execute() {}
   

std::string FulfillCustomerCommand::serialize() const { return std::string(); }
void FulfillCustomerCommand::deserialize(const std::string& data) { (void)data; }
FulfillCustomerCommand::Status FulfillCustomerCommand::getStatus() const { return status; }
void FulfillCustomerCommand::setStatus(Status s) { status = s; }
uint64_t FulfillCustomerCommand::getTargetId() const { return targetId;}
void FulfillCustomerCommand::setTargetId(uint64_t id) { targetId = id; }
