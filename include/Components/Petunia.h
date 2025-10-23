#pragma once
#include "Plant.h"

/**
 * @class Petunia
 * @brief Growth: 5 days | Water: MEDIUM | Seasons: Spring, Summer, Fall
 */
class Petunia : public Plant {
   public:
    Petunia();
    ~Petunia() override = default;

    void water() override;
    std::shared_ptr<InventoryComponent> clone() const override;
    std::shared_ptr<InventoryComponent> blueprintClone() const override;
    std::string serialize() const override;
    void deserialize(const std::string& data) override;
    std::string typeName() const override;
};
