#pragma once
#include "Plant.h"

/**
 * @class Sunflower
 * @brief Growth: 5 days | Water: HIGH | Seasons: Summer
 */
class Sunflower : public Plant {
   public:
    Sunflower();
    ~Sunflower() override = default;

    void water() override;
    std::shared_ptr<InventoryComponent> clone() const override;
    std::shared_ptr<InventoryComponent> blueprintClone() const override;
    std::string serialize() const override;
    void deserialize(const std::string& data) override;
    std::string typeName() const override;
};
