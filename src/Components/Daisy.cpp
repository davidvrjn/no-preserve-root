#include "../../include/Components/Daisy.h"

#include "../../include/Patterns/State/PlantState.h"

// Water: MEDIUM (5/day)
// Seasons: Spring, Summer
// Growth: 1d seedling + 3d growing = 4d total
// Price: R100
Daisy::Daisy() : Plant("Daisy", 100.00) {
    setWaterConsumption(5);
    setSeedlingDuration(1);
    setGrowingDuration(3);
    setCharacteristics(WaterRequirement::MEDIUM, {Season::SPRING, Season::SUMMER});
}

void Daisy::water() {
    int current = getWaterLevel();
    setWaterLevel(std::min(100, current + 40));  // Moderate water needs
}

std::shared_ptr<InventoryComponent> Daisy::clone() const {
    auto cloned = std::make_shared<Daisy>();
    cloned->setId(getId());
    cloned->setAge(getAge());
    cloned->setHealth(getHealth());
    cloned->setWaterLevel(getWaterLevel());
    return cloned;
}

std::shared_ptr<InventoryComponent> Daisy::blueprintClone() const {
    return std::make_shared<Daisy>();
}

std::string Daisy::serialize() const {
    return "Daisy";
}

void Daisy::deserialize(const std::string& data) {
    (void)data;
}

std::string Daisy::typeName() const { return "Daisy"; }
