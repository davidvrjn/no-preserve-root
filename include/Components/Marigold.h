#pragma once
#include "Plant.h"

/**
 * @class Marigold
 * @brief Growth: 3 days | Water: LOW | Seasons: Summer, Fall
 */
class Marigold : public Plant {
   public:
    Marigold();
    ~Marigold() override = default;

    void water() override;
    std::shared_ptr<InventoryComponent> clone() const override;
    std::shared_ptr<InventoryComponent> blueprintClone() const override;
    std::string serialize() const override;
    void deserialize(const std::string& data) override;
    std::string typeName() const override;
};
