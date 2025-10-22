#include "../../include/Components/SnakePlant.h"

#include "../../include/Patterns/State/PlantState.h"
// Water: VERY_LOW (2/day)
// Seasons: Year-round
// Growth: 3d seedling + 4d growing = 7d total
// Price: R120
SnakePlant::SnakePlant() : Plant("SnakePlant", 120.00) {
    setWaterConsumption(1);
    setSeedlingDuration(3);
    setGrowingDuration(4);
    setCharacteristics(WaterRequirement::VERY_LOW, {Season::YEAR_ROUND});
}

void SnakePlant::water() {
    int current = getWaterLevel();
    setWaterLevel(std::min(100, current + 20));  // Very low water needs
}

std::shared_ptr<InventoryComponent> SnakePlant::clone() const {
    auto cloned = std::make_shared<SnakePlant>();
    cloned->setId(getId());
    cloned->setAge(getAge());
    cloned->setHealth(getHealth());
    cloned->setWaterLevel(getWaterLevel());
    return cloned;
}

std::shared_ptr<InventoryComponent> SnakePlant::blueprintClone() const {
    return std::make_shared<SnakePlant>();
}

std::string SnakePlant::serialize() const {
    return "SnakePlant";
}

void SnakePlant::deserialize(const std::string& data) {
    (void)data;
}

std::string SnakePlant::typeName() const { return "SnakePlant"; }
