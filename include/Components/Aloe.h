#pragma once
#include "Plant.h"

/**
 * @class Aloe
 * @brief Growth: 6 days | Water: VERY_LOW | Seasons: Year-round
 */
class Aloe : public Plant {
   public:
    Aloe();
    ~Aloe() override = default;

    void water() override;
    std::shared_ptr<InventoryComponent> clone() const override;
    std::shared_ptr<InventoryComponent> blueprintClone() const override;
    std::string serialize() const override;
    void deserialize(const std::string& data) override;
    std::string typeName() const override;
};
