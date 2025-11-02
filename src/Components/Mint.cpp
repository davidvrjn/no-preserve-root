#include "../../include/Components/Mint.h"

#include "../../include/Core/PlantRegistry.h"
#include "../../include/Patterns/State/PlantState.h"

// Auto-register Mint type with PlantRegistry
namespace {
bool registered = []() {
    PlantRegistry::registerType("Mint", []() { return std::make_shared<Mint>(); });
    return true;
}();
}  // namespace

// Water: HIGH (7/day)
// Seasons: Spring, Summer, Fall
// Growth: 1d seedling + 2d growing = 3d total
// Price: R105
Mint::Mint() : Plant("Mint", 105.00) {
    setWaterConsumption(7);
    setSeedlingDuration(1);
    setGrowingDuration(2);
    setCharacteristics(WaterRequirement::HIGH, {Season::SPRING, Season::SUMMER, Season::FALL});
}

void Mint::water() {
    int current = getWaterLevel();
    setWaterLevel(std::min(100, current + 48));  // High water needs
}


std::shared_ptr<InventoryComponent> Mint::clone() const {
    auto cloned = std::make_shared<Mint>();
    cloned->setId(getId());
    cloned->setAge(getAge());
    cloned->setHealth(getHealth());
    cloned->setWaterLevel(getWaterLevel());
    return cloned;
}

std::shared_ptr<InventoryComponent> Mint::blueprintClone() const {
    return std::make_shared<Mint>();
}


std::string Mint::serialize() const {
    std::string baseJson = Plant::serialize();
    std::string result = "{\"type\":\"Mint\",";
    result += baseJson.substr(1);
    return result;
}

void Mint::deserialize(const std::string& data) {
    Plant::deserialize(data);  // Base class handles all fields
}

std::string Mint::typeName() const { return "Mint"; }
