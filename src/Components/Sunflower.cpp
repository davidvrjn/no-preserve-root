#include "../../include/Components/Sunflower.h"

#include "../../include/Core/PlantRegistry.h"
#include "../../include/Patterns/State/PlantState.h"

// Auto-register Sunflower type with PlantRegistry
namespace {
bool registered = []() {
    PlantRegistry::registerType("Sunflower", []() { return std::make_shared<Sunflower>(); });
    return true;
}();
}  // namespace
// Water: HIGH (7/day)
// Seasons: Summer
// Growth: 2d seedling + 3d growing = 5d total
// Price: R120
Sunflower::Sunflower() : Plant("Sunflower", 120.00) {
    setWaterConsumption(7);
    setSeedlingDuration(2);
    setGrowingDuration(3);
    setCharacteristics(WaterRequirement::HIGH, {Season::SUMMER});
}

void Sunflower::water() {
    int current = getWaterLevel();
    setWaterLevel(std::min(100, current + 45));  // High water needs
}

std::shared_ptr<InventoryComponent> Sunflower::clone() const {
    auto cloned = std::make_shared<Sunflower>();
    cloned->setId(getId());
    cloned->setAge(getAge());
    cloned->setHealth(getHealth());
    cloned->setWaterLevel(getWaterLevel());
    return cloned;
}

std::shared_ptr<InventoryComponent> Sunflower::blueprintClone() const {
    return std::make_shared<Sunflower>();
}

std::string Sunflower::serialize() const {
    std::string baseJson = Plant::serialize();
    std::string result = "{\"type\":\"Sunflower\",";
    result += baseJson.substr(1);
    return result;
}

void Sunflower::deserialize(const std::string& data) {
    Plant::deserialize(data);  // Base class handles all fields
}

std::string Sunflower::typeName() const { return "Sunflower"; }
