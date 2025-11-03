#pragma once
#include <memory>

#include "PlantDecorator.h"

/**
 * @class RibbonDecorator
 * @brief Decorator that adds a ribbon to an inventory component.
 * 
 * This decorator wraps an inventory component (typically a plant) with a decorative ribbon,
 * adding to the total price and modifying the display name to indicate the ribbon.
 * Follows the Decorator pattern to dynamically add responsibilities to objects.
 */
class RibbonDecorator : public PlantDecorator {
   public:
    RibbonDecorator(const std::shared_ptr<InventoryComponent>& component);
    ~RibbonDecorator() override = default;

    
    std::string getName() const override;
    double getPrice() const override;
    std::shared_ptr<InventoryComponent> clone() const override;
    std::shared_ptr<InventoryComponent> blueprintClone() const override;
    std::string serialize() const override;
    void deserialize(const std::string& data) override;
    std::string typeName() const override;
};
