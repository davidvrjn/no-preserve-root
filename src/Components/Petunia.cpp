#include "../../include/Components/Petunia.h"

#include "../../include/Patterns/State/PlantState.h"
// Water: MEDIUM (5/day)
// Seasons: Spring, Summer, Fall
// Growth: 2d seedling + 3d growing = 5d total
// Price: R110
Petunia::Petunia() : Plant("Petunia", 110.00) {
    setWaterConsumption(5);
    setSeedlingDuration(2);
    setGrowingDuration(3);
    setCharacteristics(WaterRequirement::MEDIUM, {Season::SPRING, Season::SUMMER, Season::FALL});
}

void Petunia::water() {
    int current = getWaterLevel();
    setWaterLevel(std::min(100, current + 40));  // Moderate water needs
}

std::shared_ptr<InventoryComponent> Petunia::clone() const {
    auto cloned = std::make_shared<Petunia>();
    cloned->setId(getId());
    cloned->setAge(getAge());
    cloned->setHealth(getHealth());
    cloned->setWaterLevel(getWaterLevel());
    return cloned;
}

std::shared_ptr<InventoryComponent> Petunia::blueprintClone() const {
    return std::make_shared<Petunia>();
}

std::string Petunia::serialize() const {
    return "Petunia";
}

void Petunia::deserialize(const std::string& data) {
    (void)data;
}

std::string Petunia::typeName() const { return "Petunia"; }
