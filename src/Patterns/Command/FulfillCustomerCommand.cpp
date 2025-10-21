#include "../../../include/Patterns/Command/FulfillCustomerCommand.h"
#include "../../../include/Patterns/Builder/PlantSpecification.h"
#include <utility>

FulfillCustomerCommand::FulfillCustomerCommand(std::unique_ptr<PlantSpecification> spec,
	const std::shared_ptr<Inventory>& inventory,
	const std::shared_ptr<Customer>& customer)
{
	(void)spec; (void)inventory; (void)customer;
}

void FulfillCustomerCommand::execute() { }

std::string FulfillCustomerCommand::serialize() const { return std::string(); }
void FulfillCustomerCommand::deserialize(const std::string& data) { (void)data; }
FulfillCustomerCommand::Status FulfillCustomerCommand::getStatus() const { return Status::Pending; }
void FulfillCustomerCommand::setStatus(Status s) { (void)s; }
uint64_t FulfillCustomerCommand::getTargetId() const { return 0; }
void FulfillCustomerCommand::setTargetId(uint64_t id) { (void)id; }

