#include "../../include/Components/Sunflower.h"

#include "../../include/Patterns/State/PlantState.h"
// Water: HIGH (7/day)
// Seasons: Summer
// Growth: 2d seedling + 3d growing = 5d total
// Price: R115
Sunflower::Sunflower() : Plant("Sunflower", 120.00) {
    setWaterConsumption(7);
    setSeedlingDuration(2);
    setGrowingDuration(3);
    setCharacteristics(WaterRequirement::HIGH, {Season::SUMMER});
}

void Sunflower::water() {
    int current = getWaterLevel();
    setWaterLevel(std::min(100, current + 45));  // High water needs
}

std::shared_ptr<InventoryComponent> Sunflower::clone() const {
    auto cloned = std::make_shared<Sunflower>();
    cloned->setId(getId());
    cloned->setAge(getAge());
    cloned->setHealth(getHealth());
    cloned->setWaterLevel(getWaterLevel());
    return cloned;
}

std::shared_ptr<InventoryComponent> Sunflower::blueprintClone() const {
    return std::make_shared<Sunflower>();
}

std::string Sunflower::serialize() const {
    return "Sunflower";
}

void Sunflower::deserialize(const std::string& data) {
    (void)data;
}

std::string Sunflower::typeName() const { return "Sunflower"; }
