#include "../../include/Components/Marigold.h"

#include "../../include/Core/PlantRegistry.h"
#include "../../include/Patterns/State/PlantState.h"

// Auto-register Marigold type with PlantRegistry
namespace {
bool registered = []() {
    PlantRegistry::registerType("Marigold", []() { return std::make_shared<Marigold>(); });
    return true;
}();
}  // namespace
// Water: LOW (4/day)
// Seasons: Summer, Fall
// Growth: 1d seedling + 2d growing = 3d total
// Price: R85
Marigold::Marigold() : Plant("Marigold", 85.00) {
    setWaterConsumption(4);
    setSeedlingDuration(1);
    setGrowingDuration(2);
    setCharacteristics(WaterRequirement::LOW, {Season::SUMMER, Season::FALL});
}

void Marigold::water() {
    int current = getWaterLevel();
    setWaterLevel(std::min(100, current + 35));  // Low-moderate water
}

std::shared_ptr<InventoryComponent> Marigold::clone() const {
    auto cloned = std::make_shared<Marigold>();
    cloned->setId(getId());
    cloned->setAge(getAge());
    cloned->setHealth(getHealth());
    cloned->setWaterLevel(getWaterLevel());
    return cloned;
}

std::shared_ptr<InventoryComponent> Marigold::blueprintClone() const {
    return std::make_shared<Marigold>();
}

std::string Marigold::serialize() const {
    std::string baseJson = Plant::serialize();
    std::string result = "{\"type\":\"Marigold\",";
    result += baseJson.substr(1);
    return result;
}

void Marigold::deserialize(const std::string& data) {
    Plant::deserialize(data);  // Base class handles all fields
}

std::string Marigold::typeName() const { return "Marigold"; }
