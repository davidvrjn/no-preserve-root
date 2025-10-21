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
 * This command holds the PlantSpecification, and references to the Inventory
 * and Customer so it can locate and allocate the requested plant(s).
 */
class FulfillCustomerCommand : public Command {
private:
	std::unique_ptr<PlantSpecification> spec; 
	Inventory* inventory;
	Customer* customer;

public:
	FulfillCustomerCommand(std::unique_ptr<PlantSpecification> spec, Inventory* inventory, Customer* customer);
	~FulfillCustomerCommand();

	void execute() override;
};

