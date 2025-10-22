#pragma once
#include <memory>

#include "PlantDecorator.h"

class PotDecorator : public PlantDecorator {
   public:
    PotDecorator(const std::shared_ptr<InventoryComponent>& component);
    ~PotDecorator() override = default;

    // Overrides to add the pot's price and modify the name.
    std::string getName() const override;
    double getPrice() const override;
    std::shared_ptr<InventoryComponent> blueprintClone() const override;
    std::string serialize() const override;
    void deserialize(const std::string& data) override;
    std::string typeName() const override;
};
