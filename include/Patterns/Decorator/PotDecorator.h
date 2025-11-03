#pragma once
#include <memory>

#include "PlantDecorator.h"

/**
 * @class PotDecorator
 * @brief Decorator that adds a pot to an inventory component.
 * 
 * This decorator wraps an inventory component (typically a plant) with a pot,
 * adding to the total price and modifying the display name to indicate the pot.
 * Follows the Decorator pattern to dynamically add responsibilities to objects.
 */
class PotDecorator : public PlantDecorator {
   public:
    PotDecorator(const std::shared_ptr<InventoryComponent>& component);
    ~PotDecorator() override = default;

    // Overrides to add the pot's price and modify the name.
    std::string getName() const override;
    double getPrice() const override;
    std::shared_ptr<InventoryComponent> clone() const override;
    std::shared_ptr<InventoryComponent> blueprintClone() const override;
    std::string serialize() const override;
    void deserialize(const std::string& data) override;
    std::string typeName() const override;
};
