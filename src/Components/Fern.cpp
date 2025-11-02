#include "../../include/Components/Fern.h"

#include "../../include/Core/PlantRegistry.h"
#include "../../include/Patterns/State/PlantState.h"

// Auto-register Fern type with PlantRegistry
namespace {
bool registered = []() {
    PlantRegistry::registerType("Fern", []() { return std::make_shared<Fern>(); });
    return true;
}();
}  // namespace
// Water: HIGH (8/day)
// Seasons: Year-round
// Growth: 2d seedling + 4d growing = 6d total
// Price: R130
Fern::Fern() : Plant("Fern", 130.00) {
    setWaterConsumption(8);
    setSeedlingDuration(2);
    setGrowingDuration(4);
    setCharacteristics(WaterRequirement::HIGH, {Season::YEAR_ROUND});
}


void Fern::water() {
    int current = getWaterLevel();
    setWaterLevel(std::min(100, current + 50));  // High water needs
}


std::shared_ptr<InventoryComponent> Fern::clone() const {
    auto cloned = std::make_shared<Fern>();
    cloned->setId(getId());
    cloned->setAge(getAge());
    cloned->setHealth(getHealth());
    cloned->setWaterLevel(getWaterLevel());
    return cloned;
}


std::shared_ptr<InventoryComponent> Fern::blueprintClone() const {
    return std::make_shared<Fern>();
}


std::string Fern::serialize() const {
    std::string baseJson = Plant::serialize();
    std::string result = "{\"type\":\"Fern\",";
    result += baseJson.substr(1);
    return result;
}


void Fern::deserialize(const std::string& data) {
    Plant::deserialize(data);  // Base class handles all fields
}


std::string Fern::typeName() const { return "Fern"; }
