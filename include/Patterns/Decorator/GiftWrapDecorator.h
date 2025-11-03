#pragma once
#include <memory>

#include "PlantDecorator.h"

/**
 * @class GiftWrapDecorator
 * @brief Decorator that adds gift wrapping to an inventory component.
 * 
 * This decorator wraps an inventory component (typically a plant) with gift wrapping,
 * adding to the total price and modifying the display name to indicate the gift wrap.
 * Follows the Decorator pattern to dynamically add responsibilities to objects.
 */
class GiftWrapDecorator : public PlantDecorator {
   public:
    GiftWrapDecorator(const std::shared_ptr<InventoryComponent>& component);
    ~GiftWrapDecorator() override = default;

    // Overrides to add the gift wrap's price and modify the name.
    std::string getName() const override;
    double getPrice() const override;
    std::shared_ptr<InventoryComponent> blueprintClone() const override;
    std::shared_ptr<InventoryComponent> clone() const override;
    std::string serialize() const override;
    void deserialize(const std::string& data) override;
    std::string typeName() const override;
};
