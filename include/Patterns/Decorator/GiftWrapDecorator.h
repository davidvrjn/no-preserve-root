#pragma once
#include "PlantDecorator.h"
#include <memory>

class GiftWrapDecorator : public PlantDecorator {
public:
    GiftWrapDecorator(const std::shared_ptr<InventoryComponent>& component);
    ~GiftWrapDecorator() override = default;

    // Overrides to add the gift wrap's price and modify the name.
    std::string getName() const override;
    double getPrice() const override;
    std::shared_ptr<InventoryComponent> blueprintClone() const override;
    std::string serialize() const override;
    void deserialize(const std::string& data) override;
    std::string typeName() const override;
};
