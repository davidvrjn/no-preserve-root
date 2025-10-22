#pragma once
#include "Plant.h"

/**
 * @class Basil
 * @brief Growth: 3 days | Water: MEDIUM | Seasons: Summer
 */
class Basil : public Plant {
   public:
    Basil();
    ~Basil() override = default;

    void water() override;
    std::shared_ptr<InventoryComponent> clone() const override;
    std::shared_ptr<InventoryComponent> blueprintClone() const override;
    std::string serialize() const override;
    void deserialize(const std::string& data) override;
    std::string typeName() const override;
};
