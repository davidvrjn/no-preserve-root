#pragma once
#include "Plant.h"

/**
 * @class Bamboo
 * @brief Growth: 3 days | Water: MEDIUM | Seasons: Year-round
 */
class Bamboo : public Plant {
   public:
    Bamboo();
    ~Bamboo() override = default;

    void water() override;
    std::shared_ptr<InventoryComponent> clone() const override;
    std::shared_ptr<InventoryComponent> blueprintClone() const override;
    std::string serialize() const override;
    void deserialize(const std::string& data) override;
    std::string typeName() const override;
};
