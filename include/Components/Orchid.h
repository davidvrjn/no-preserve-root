#pragma once
#include "Plant.h"

/**
 * @class Orchid
 * @brief Growth: 7 days | Water: LOW | Seasons: Year-round
 */
class Orchid : public Plant {
   public:
    Orchid();
    ~Orchid() override = default;

    void water() override;
    std::shared_ptr<InventoryComponent> clone() const override;
    std::shared_ptr<InventoryComponent> blueprintClone() const override;
    std::string serialize() const override;
    void deserialize(const std::string& data) override;
    std::string typeName() const override;
};
