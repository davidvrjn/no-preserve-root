#include "../../include/Components/Orchid.h"

#include "../../include/Patterns/State/PlantState.h"
// Water: LOW (4/day)
// Seasons: Year-round
// Growth: 3d seedling + 4d growing = 7d total
// Price: R145
Orchid::Orchid() : Plant("Orchid", 145.00) {
    setWaterConsumption(4);
    setSeedlingDuration(3);
    setGrowingDuration(4);
    setCharacteristics(WaterRequirement::LOW, {Season::YEAR_ROUND});
}

void Orchid::water() {
    int current = getWaterLevel();
    setWaterLevel(std::min(100, current + 35));  // Moderate, careful watering
}

std::shared_ptr<InventoryComponent> Orchid::clone() const {
    auto cloned = std::make_shared<Orchid>();
    cloned->setId(getId());
    cloned->setAge(getAge());
    cloned->setHealth(getHealth());
    cloned->setWaterLevel(getWaterLevel());
    return cloned;
}

std::shared_ptr<InventoryComponent> Orchid::blueprintClone() const {
    return std::make_shared<Orchid>();
}

std::string Orchid::serialize() const {
    return "Orchid";
}

void Orchid::deserialize(const std::string& data) {
    (void)data;
}

std::string Orchid::typeName() const { return "Orchid"; }
