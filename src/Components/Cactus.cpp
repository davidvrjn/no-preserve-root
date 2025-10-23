#include "../../include/Components/Cactus.h"

#include "../../include/Patterns/State/PlantState.h"
// Water: VERY_LOW (1/day)
// Seasons: Year-round
// Growth: 3d seedling + 4d growing = 7d total
// Price: R120
Cactus::Cactus() : Plant("Cactus", 120.00) {
    setWaterConsumption(1);
    setSeedlingDuration(3);
    setGrowingDuration(4);
    setCharacteristics(WaterRequirement::VERY_LOW, {Season::YEAR_ROUND});
}

void Cactus::water() {
    int current = getWaterLevel();
    setWaterLevel(std::min(100, current + 18));  // Very low water needs
}

std::shared_ptr<InventoryComponent> Cactus::clone() const {
    auto cloned = std::make_shared<Cactus>();
    cloned->setId(getId());
    cloned->setAge(getAge());
    cloned->setHealth(getHealth());
    cloned->setWaterLevel(getWaterLevel());
    return cloned;
}

std::shared_ptr<InventoryComponent> Cactus::blueprintClone() const {
    return std::make_shared<Cactus>();
}

std::string Cactus::serialize() const {
    return "Cactus";
}

void Cactus::deserialize(const std::string& data) {
    (void)data;
}

std::string Cactus::typeName() const { return "Cactus"; }
