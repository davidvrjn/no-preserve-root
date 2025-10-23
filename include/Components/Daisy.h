#pragma once
#include "Plant.h"

/**
 * @class Daisy
 * @brief Growth: 4 days | Water: MEDIUM | Seasons: Spring, Summer
 */
class Daisy : public Plant {
   public:
    Daisy();
    ~Daisy() override = default;

    void water() override;
    std::shared_ptr<InventoryComponent> clone() const override;
    std::shared_ptr<InventoryComponent> blueprintClone() const override;
    std::string serialize() const override;
    void deserialize(const std::string& data) override;
    std::string typeName() const override;
};
