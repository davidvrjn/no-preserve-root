#include "../../include/Components/Ivy.h"

#include "../../include/Patterns/State/PlantState.h"
// Water: LOW (4/day)
// Seasons: Year-round
// Growth: 2d seedling + 3d growing = 5d total
// Price: R110
Ivy::Ivy() : Plant("Ivy", 110.00) {
    setWaterConsumption(4);
    setSeedlingDuration(2);
    setGrowingDuration(3);
    setCharacteristics(WaterRequirement::LOW, {Season::YEAR_ROUND});
}

void Ivy::water() {
    int current = getWaterLevel();
    setWaterLevel(std::min(100, current + 38));  // Low-moderate water
}

std::shared_ptr<InventoryComponent> Ivy::clone() const {
    auto cloned = std::make_shared<Ivy>();
    cloned->setId(getId());
    cloned->setAge(getAge());
    cloned->setHealth(getHealth());
    cloned->setWaterLevel(getWaterLevel());
    return cloned;
}

std::shared_ptr<InventoryComponent> Ivy::blueprintClone() const {
    return std::make_shared<Ivy>();
}

std::string Ivy::serialize() const {
    return "Ivy";
}

void Ivy::deserialize(const std::string& data) {
    (void)data;
}

std::string Ivy::typeName() const { return "Ivy"; }
