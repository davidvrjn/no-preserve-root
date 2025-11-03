
#pragma once
#include <memory>

#include "Command.h"

// Forward declarations
struct PlantSpecification;
class Inventory;
class Customer;
class InventoryComponent;
class Nursery;

/**
 * @class FulfillCustomerCommand
 * @brief A concrete Command to fulfill a customer's plant request.
 *
 * This command holds the PlantSpecification, and non-owning references to Inventory,
 * Customer, and Nursery. When executed, it updates the Nursery's business metrics
 * (money and reputation) based on the outcome.
 */
class FulfillCustomerCommand : public Command {
   private:
    std::unique_ptr<PlantSpecification> spec;
    std::weak_ptr<Inventory> inventory;
    std::weak_ptr<Nursery> nursery;
    Status status;
    uint64_t targetId;
    std::shared_ptr<InventoryComponent> decoratedPlant;
    double salePrice = 0.0;

   public:
    FulfillCustomerCommand(std::unique_ptr<PlantSpecification> spec,
                           const std::shared_ptr<Inventory>& inventory,
                           const std::shared_ptr<Nursery>& nursery);
    ~FulfillCustomerCommand() override = default;

    void execute() override;

    Status getStatus() const override;
    void setStatus(Status s) override;
    uint64_t getTargetId() const override;
    void setTargetId(uint64_t id) override;

    std::shared_ptr<InventoryComponent> getDecoratedPlant() const { return decoratedPlant; }
    double getSalePrice() const { return salePrice; }
    // Human-readable description for UI
    std::string toString() const override;
};
