
#pragma once
#include "Command.h"
#include <memory>

// Forward declarations
class PlantSpecification;
class Inventory;
class Customer;

/**
 * @class FulfillCustomerCommand
 * @brief A concrete Command to fulfill a customer's plant request.
 *
 * This command holds the PlantSpecification, and non-owning references to the Inventory
 * and Customer so it can locate and allocate the requested plant(s). Non-owning references
 * are stored as weak_ptrs and will be checked at execution time.
 */
class FulfillCustomerCommand : public Command {
private:
	std::unique_ptr<PlantSpecification> spec; 
	std::weak_ptr<Inventory> inventory;
	std::weak_ptr<Customer> customer;

public:
	FulfillCustomerCommand(std::unique_ptr<PlantSpecification> spec,
				   const std::shared_ptr<Inventory>& inventory,
				   const std::shared_ptr<Customer>& customer);
	~FulfillCustomerCommand() override = default;

	void execute() override;

	std::string serialize() const override;
	void deserialize(const std::string& data) override;
	Status getStatus() const override;
	void setStatus(Status s) override;
	uint64_t getTargetId() const override;
	void setTargetId(uint64_t id) override;
};

