#include "../../include/Components/Lavender.h"

#include "../../include/Core/PlantRegistry.h"
#include "../../include/Patterns/State/PlantState.h"

// Auto-register Lavender type with PlantRegistry
namespace {
bool registered = []() {
    PlantRegistry::registerType("Lavender", []() { return std::make_shared<Lavender>(); });
    return true;
}();
}  // namespace

// Water: LOW (3/day)
// Seasons: Spring, Summer
// Growth: 2d seedling + 3d growing = 5d total
// Price: R100
Lavender::Lavender() : Plant("Lavender", 100.00) {
    setWaterConsumption(3);
    setSeedlingDuration(2);
    setGrowingDuration(3);
    setCharacteristics(WaterRequirement::LOW, {Season::SPRING, Season::SUMMER});
}

void Lavender::water() {
    int current = getWaterLevel();
    setWaterLevel(std::min(100, current + 30));  // Low water needs, adds 30
}

std::shared_ptr<InventoryComponent> Lavender::clone() const {
    auto cloned = std::make_shared<Lavender>();
    cloned->setId(getId());
    cloned->setAge(getAge());
    cloned->setHealth(getHealth());
    cloned->setWaterLevel(getWaterLevel());
    return cloned;
}

std::shared_ptr<InventoryComponent> Lavender::blueprintClone() const {
    return std::make_shared<Lavender>();
}

std::string Lavender::serialize() const {
    std::string baseJson = Plant::serialize();
    std::string result = "{\"type\":\"Lavender\",";
    result += baseJson.substr(1);
    return result;
}

void Lavender::deserialize(const std::string& data) { (void)data; }

std::string Lavender::typeName() const { return "Lavender"; }
