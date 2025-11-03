#include "../../include/Components/Daisy.h"

#include "../../include/Core/PlantRegistry.h"
#include "../../include/Patterns/State/PlantState.h"

// Auto-register Daisy type with PlantRegistry
namespace {
bool registered = []() {
    PlantRegistry::registerType("Daisy", []() { return std::make_shared<Daisy>(); });
    return true;
}();
}  // namespace

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
    std::string baseJson = Plant::serialize();
    std::string result = "{\"type\":\"Daisy\",";
    result += baseJson.substr(1);
    return result;
}

void Daisy::deserialize(const std::string& data) {
    Plant::deserialize(data);  // Base class handles all fields
}

std::string Daisy::typeName() const { return "Daisy"; }
