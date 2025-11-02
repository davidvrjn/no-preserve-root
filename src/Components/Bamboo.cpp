#include "../../include/Components/Bamboo.h"

#include "../../include/Core/PlantRegistry.h"
#include "../../include/Patterns/State/PlantState.h"

// Auto-register Bamboo type with PlantRegistry
namespace {
bool registered = []() {
    PlantRegistry::registerType("Bamboo", []() { return std::make_shared<Bamboo>(); });
    return true;
}();
}  // namespace
// Water: MEDIUM (6/day)
// Seasons: Year-round
// Growth: 1d seedling + 2d growing = 3d total
// Price: R110
Bamboo::Bamboo() : Plant("Bamboo", 110.00) {
    setWaterConsumption(6);
    setSeedlingDuration(1);
    setGrowingDuration(2);
    setCharacteristics(WaterRequirement::MEDIUM, {Season::YEAR_ROUND});
}


void Bamboo::water() {
    int current = getWaterLevel();
    setWaterLevel(std::min(100, current + 42));  // Moderate-high water needs
}


std::shared_ptr<InventoryComponent> Bamboo::clone() const {
    auto cloned = std::make_shared<Bamboo>();
    cloned->setId(getId());
    cloned->setAge(getAge());
    cloned->setHealth(getHealth());
    cloned->setWaterLevel(getWaterLevel());
    return cloned;
}


std::shared_ptr<InventoryComponent> Bamboo::blueprintClone() const {
    return std::make_shared<Bamboo>();
}


std::string Bamboo::serialize() const {
    std::string baseJson = Plant::serialize();
    std::string result = "{\"type\":\"Bamboo\",";
    result += baseJson.substr(1);
    return result;
}


void Bamboo::deserialize(const std::string& data) {
    Plant::deserialize(data);  // Base class handles all fields
}


std::string Bamboo::typeName() const { return "Bamboo"; }
