#include "../../include/Components/Ivy.h"

#include "../../include/Core/PlantRegistry.h"
#include "../../include/Patterns/State/PlantState.h"

// Auto-register Ivy type with PlantRegistry
namespace {
bool registered = []() {
    PlantRegistry::registerType("Ivy", []() { return std::make_shared<Ivy>(); });
    return true;
}();
}  // namespace
// Water: LOW (4/day)
// Seasons: Year-round
// Growth: 2d seedling + 3d growing = 5d total
// Price: R11
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

std::shared_ptr<InventoryComponent> Ivy::blueprintClone() const { return std::make_shared<Ivy>(); }

std::string Ivy::serialize() const {
    std::string baseJson = Plant::serialize();
    std::string result = "{\"type\":\"Ivy\",";
    result += baseJson.substr(1);
    return result;
}

void Ivy::deserialize(const std::string& data) {
    Plant::deserialize(data);  // Base class handles all fields
}

std::string Ivy::typeName() const { return "Ivy"; }
