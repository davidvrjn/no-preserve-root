#pragma once
#include "Plant.h"
#include <memory>

class Cactus : public Plant {
public:
    Cactus(const std::string& name, double price);
    ~Cactus() override = default;

    void water() override;
    std::shared_ptr<InventoryComponent> clone() const override;
    std::shared_ptr<InventoryComponent> blueprintClone() const override;
    std::string serialize() const override;
    void deserialize(const std::string& data) override;
    std::string typeName() const override;
};
