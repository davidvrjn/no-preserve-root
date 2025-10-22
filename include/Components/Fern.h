#pragma once
#include "Plant.h"

/**
 * @class Fern
 * @brief Growth: 6 days | Water: HIGH | Seasons: Year-round
 */
class Fern : public Plant {
   public:
    Fern();
    ~Fern() override = default;

    void water() override;
    std::shared_ptr<InventoryComponent> clone() const override;
    std::shared_ptr<InventoryComponent> blueprintClone() const override;
    std::string serialize() const override;
    void deserialize(const std::string& data) override;
    std::string typeName() const override;
};
