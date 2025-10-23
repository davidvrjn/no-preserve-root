#include "../../include/Components/Lavender.h"

#include "../../include/Patterns/State/PlantState.h"

// Water: LOW (4/day)
// Seasons: Spring, Summer
// Growth: 2d seedling + 3d growing = 5d total
// Price: R100
Lavender::Lavender() : Plant("Lavender", 100.00) {
    setWaterConsumption(3);
    setSeedlingDuration(2);
    setGrowingDuration(3);
    setCharacteristics(WaterRequirement::LOW, {Season::SPRING, Season::SUMMER});
}

void Lavender::water() {
    int current = getWaterLevel();
    setWaterLevel(std::min(100, current + 30));  // Low water needs, adds 30
}

std::shared_ptr<InventoryComponent> Lavender::clone() const {
    auto cloned = std::make_shared<Lavender>();
    cloned->setId(getId());
    cloned->setAge(getAge());
    cloned->setHealth(getHealth());
    cloned->setWaterLevel(getWaterLevel());
    return cloned;
}

std::shared_ptr<InventoryComponent> Lavender::blueprintClone() const {
    return std::make_shared<Lavender>();
}

std::string Lavender::serialize() const {
    return "Lavender";  // TODO: Implement proper JSON serialization
}

void Lavender::deserialize(const std::string& data) {
    (void)data;  // TODO: Implement proper JSON deserialization
}

std::string Lavender::typeName() const { return "Lavender"; }
