#include "../../include/Components/Rose.h"

#include "../../include/Patterns/State/PlantState.h"

// Water: MEDIUM (6/day)
// Seasons: Spring, Summer, Fall
// Growth: 2d seedling + 4d growing = 6d total
// Price: R135
Rose::Rose() : Plant("Rose", 135.00) {
    setWaterConsumption(6);
    setSeedlingDuration(2);
    setGrowingDuration(4);
    setCharacteristics(WaterRequirement::MEDIUM, {Season::SPRING, Season::SUMMER, Season::FALL});
}

void Rose::water() {
    int current = getWaterLevel();
    setWaterLevel(std::min(100, current + 42));  // Moderate-high water needs
}

std::shared_ptr<InventoryComponent> Rose::clone() const {
    auto cloned = std::make_shared<Rose>();
    cloned->setId(getId());
    cloned->setAge(getAge());
    cloned->setHealth(getHealth());
    cloned->setWaterLevel(getWaterLevel());
    return cloned;
}

std::shared_ptr<InventoryComponent> Rose::blueprintClone() const {
    return std::make_shared<Rose>();
}

std::string Rose::serialize() const {
    // Use Plant's base serialization and add type identifier
    std::string baseJson = Plant::serialize();
    
    // Insert type at the beginning of the JSON object
    // baseJson is like: {"id":123,...}
    // We want: {"type":"Rose","id":123,...}
    std::string result = "{\"type\":\"Rose\",";
    result += baseJson.substr(1);  // Skip the opening brace from base JSON
    
    return result;
}

void Rose::deserialize(const std::string& data) {
    (void)data;
}
 
std::string Rose::typeName() const { return "Rose"; }
