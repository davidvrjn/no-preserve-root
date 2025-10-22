#pragma once
#include "Plant.h"

/**
 * @class Ivy
 * @brief Growth: 5 days | Water: LOW | Seasons: Year-round
 */
class Ivy : public Plant {
   public:
    Ivy();
    ~Ivy() override = default;

    void water() override;
    std::shared_ptr<InventoryComponent> clone() const override;
    std::shared_ptr<InventoryComponent> blueprintClone() const override;
    std::string serialize() const override;
    void deserialize(const std::string& data) override;
    std::string typeName() const override;
};
