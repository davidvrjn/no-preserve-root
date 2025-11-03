#include "../../include/Components/Aloe.h"

#include "../../include/Core/PlantRegistry.h"
#include "../../include/Patterns/State/PlantState.h"

// Auto-register Aloe type with PlantRegistry
namespace {
bool registered = []() {
    PlantRegistry::registerType("Aloe", []() { return std::make_shared<Aloe>(); });
    return true;
}();
}  // namespace
// Water: VERY_LOW (3/day)
// Seasons: Year-round
// Growth: 2d seedling + 4d growing = 6d total
// Price: R110
Aloe::Aloe() : Plant("Aloe", 110.00) {
    setWaterConsumption(2);
    setSeedlingDuration(2);
    setGrowingDuration(4);
    setCharacteristics(WaterRequirement::VERY_LOW, {Season::YEAR_ROUND});
}

void Aloe::water() {
    int current = getWaterLevel();
    setWaterLevel(std::min(100, current + 22));  // Very low water needs
}

std::shared_ptr<InventoryComponent> Aloe::clone() const {
    auto cloned = std::make_shared<Aloe>();
    cloned->setId(getId());
    cloned->setAge(getAge());
    cloned->setHealth(getHealth());
    cloned->setWaterLevel(getWaterLevel());
    return cloned;
}

std::shared_ptr<InventoryComponent> Aloe::blueprintClone() const {
    return std::make_shared<Aloe>();
}

std::string Aloe::serialize() const {
    std::string baseJson = Plant::serialize();
    std::string result = "{\"type\":\"Aloe\",";
    result += baseJson.substr(1);
    return result;
}

void Aloe::deserialize(const std::string& data) {
    Plant::deserialize(data);  // Base class handles all fields
}

std::string Aloe::typeName() const { return "Aloe"; }
