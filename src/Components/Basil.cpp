#include "../../include/Components/Basil.h"

#include "../../include/Core/PlantRegistry.h"
#include "../../include/Patterns/State/PlantState.h"

// Auto-register Basil type with PlantRegistry
namespace {
bool registered = []() {
    PlantRegistry::registerType("Basil", []() { return std::make_shared<Basil>(); });
    return true;
}();
}  // namespace
// Water: MEDIUM (6/day)
// Seasons: Summer
// Growth: 1d seedling + 2d growing = 3d total
// Price: R90
Basil::Basil() : Plant("Basil", 90.00) {
    setWaterConsumption(6);
    setSeedlingDuration(1);
    setGrowingDuration(2);
    setCharacteristics(WaterRequirement::MEDIUM, {Season::SUMMER});
}


void Basil::water() {
    int current = getWaterLevel();
    setWaterLevel(std::min(100, current + 43));  // Moderate-high water
}


std::shared_ptr<InventoryComponent> Basil::clone() const {
    auto cloned = std::make_shared<Basil>();
    cloned->setId(getId());
    cloned->setAge(getAge());
    cloned->setHealth(getHealth());
    cloned->setWaterLevel(getWaterLevel());
    return cloned;
}


std::shared_ptr<InventoryComponent> Basil::blueprintClone() const {
    return std::make_shared<Basil>();
}


std::string Basil::serialize() const {
    std::string baseJson = Plant::serialize();
    std::string result = "{\"type\":\"Basil\",";
    result += baseJson.substr(1);
    return result;
}


void Basil::deserialize(const std::string& data) {
    Plant::deserialize(data);  // Base class handles all fields
}


std::string Basil::typeName() const { return "Basil"; }
