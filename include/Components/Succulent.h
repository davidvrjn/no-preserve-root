#pragma once
#include "Plant.h"

/**
 * @class Succulent
 * @brief Growth: 5 days | Water: VERY_LOW | Seasons: Spring, Summer
 */
class Succulent : public Plant {
   public:
    Succulent();
    ~Succulent() override = default;

    void water() override;
    std::shared_ptr<InventoryComponent> clone() const override;
    std::shared_ptr<InventoryComponent> blueprintClone() const override;
    std::string serialize() const override;
    void deserialize(const std::string& data) override;
    std::string typeName() const override;
};
