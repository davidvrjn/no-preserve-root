#include "../../include/Components/Petunia.h"

#include "../../include/Core/PlantRegistry.h"
#include "../../include/Patterns/State/PlantState.h"

// Auto-register Petunia type with PlantRegistry
namespace {
bool registered = []() {
    PlantRegistry::registerType("Petunia", []() { return std::make_shared<Petunia>(); });
    return true;
}();
}  // namespace
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
    std::string baseJson = Plant::serialize();
    std::string result = "{\"type\":\"Petunia\",";
    result += baseJson.substr(1);
    return result;
}

void Petunia::deserialize(const std::string& data) {
    Plant::deserialize(data);  // Base class handles all fields
}

std::string Petunia::typeName() const { return "Petunia"; }
