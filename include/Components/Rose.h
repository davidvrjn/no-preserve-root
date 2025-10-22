#pragma once
#include <memory>

#include "Plant.h"

class Rose : public Plant {
   public:
    Rose(const std::string& name, double price);
    ~Rose() override = default;

    void water() override;
    std::shared_ptr<InventoryComponent> clone() const override;
    std::shared_ptr<InventoryComponent> blueprintClone() const override;
    std::string serialize() const override;
    void deserialize(const std::string& data) override;
    std::string typeName() const override;
};
