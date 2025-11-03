#pragma once
#include "Plant.h"

/**
 * @class Mint
 * @brief Growth: 3 days | Water: HIGH | Seasons: Spring, Summer, Fall
 */
class Mint : public Plant {
   public:
    Mint();
    ~Mint() override = default;

    void water() override;
    std::shared_ptr<InventoryComponent> clone() const override;
    std::shared_ptr<InventoryComponent> blueprintClone() const override;
    std::string serialize() const override;
    void deserialize(const std::string& data) override;
    std::string typeName() const override;
};
