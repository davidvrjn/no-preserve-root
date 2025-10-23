#pragma once
#include "Plant.h"

/**
 * @class Cactus
 * @brief Growth: 7 days | Water: VERY_LOW | Seasons: Year-round
 */
class Cactus : public Plant {
   public:
    Cactus();
    ~Cactus() override = default;

    void water() override;
    std::shared_ptr<InventoryComponent> clone() const override;
    std::shared_ptr<InventoryComponent> blueprintClone() const override;
    std::string serialize() const override;
    void deserialize(const std::string& data) override;
    std::string typeName() const override;
};
