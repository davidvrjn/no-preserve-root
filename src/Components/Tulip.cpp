#include "../../include/Components/Tulip.h"

#include "../../include/Core/PlantRegistry.h"
#include "../../include/Patterns/State/PlantState.h"

// Auto-register Tulip type with PlantRegistry
namespace {
bool registered = []() {
    PlantRegistry::registerType("Tulip", []() { return std::make_shared<Tulip>(); });
    return true;
}();
}  // namespace

// Water: MEDIUM (5/day)
// Seasons: Spring
// Growth: 2d seedling + 3d growing = 5d total
// Price: R105
Tulip::Tulip() : Plant("Tulip", 105.00) {
    setWaterConsumption(5);
    setSeedlingDuration(2);
    setGrowingDuration(3);
    setCharacteristics(WaterRequirement::MEDIUM, {Season::SPRING});
}

void Tulip::water() {
    int current = getWaterLevel();
    setWaterLevel(std::min(100, current + 40));  // Moderate water needs
}


std::shared_ptr<InventoryComponent> Tulip::clone() const {
    auto cloned = std::make_shared<Tulip>();
    cloned->setId(getId());
    cloned->setAge(getAge());
    cloned->setHealth(getHealth());
    cloned->setWaterLevel(getWaterLevel());
    return cloned;
}

std::shared_ptr<InventoryComponent> Tulip::blueprintClone() const {
    return std::make_shared<Tulip>();
}

std::string Tulip::serialize() const {
    std::string baseJson = Plant::serialize();
    std::string result = "{\"type\":\"Tulip\",";
    result += baseJson.substr(1);
    return result;
}

void Tulip::deserialize(const std::string& data) {
    Plant::deserialize(data);  // Base class handles all fields
}

std::string Tulip::typeName() const { return "Tulip"; }
