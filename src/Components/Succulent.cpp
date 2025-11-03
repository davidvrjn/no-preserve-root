#include "../../include/Components/Succulent.h"

#include "../../include/Core/PlantRegistry.h"
#include "../../include/Patterns/State/PlantState.h"

// Auto-register Succulent type with PlantRegistry
namespace {
bool registered = []() {
    PlantRegistry::registerType("Succulent", []() { return std::make_shared<Succulent>(); });
    return true;
}();
}  // namespace

// Water: VERY_LOW (2/day)
// Seasons: Spring, Summer
// Growth: 2d seedling + 3d growing = 5d total
// Price: R95
Succulent::Succulent() : Plant("Succulent", 95.00) {
    setWaterConsumption(2);
    setSeedlingDuration(2);
    setGrowingDuration(3);
    setCharacteristics(WaterRequirement::VERY_LOW, {Season::SPRING, Season::SUMMER});
}

void Succulent::water() {
    int current = getWaterLevel();
    setWaterLevel(std::min(100, current + 25));  // Very low water needs
}

std::shared_ptr<InventoryComponent> Succulent::clone() const {
    auto cloned = std::make_shared<Succulent>();
    cloned->setId(getId());
    cloned->setAge(getAge());
    cloned->setHealth(getHealth());
    cloned->setWaterLevel(getWaterLevel());
    return cloned;
}

std::shared_ptr<InventoryComponent> Succulent::blueprintClone() const {
    return std::make_shared<Succulent>();
}

std::string Succulent::serialize() const {
    std::string baseJson = Plant::serialize();
    std::string result = "{\"type\":\"Succulent\",";
    result += baseJson.substr(1);
    return result;
}

void Succulent::deserialize(const std::string& data) {
    Plant::deserialize(data);  // Base class handles all fields
}

std::string Succulent::typeName() const { return "Succulent"; }
