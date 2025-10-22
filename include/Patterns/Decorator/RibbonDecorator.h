#pragma once
#include <memory>

#include "PlantDecorator.h"

class RibbonDecorator : public PlantDecorator {
   public:
    RibbonDecorator(const std::shared_ptr<InventoryComponent>& component);
    ~RibbonDecorator() override = default;

    // Overrides to add the ribbon's price and modify the name.
    std::string getName() const override;
    double getPrice() const override;
    std::shared_ptr<InventoryComponent> blueprintClone() const override;
    std::string serialize() const override;
    void deserialize(const std::string& data) override;
    std::string typeName() const override;
};
