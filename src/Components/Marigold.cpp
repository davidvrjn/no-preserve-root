#include "../../include/Components/Marigold.h"

#include "../../include/Patterns/State/PlantState.h"

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
    return "Marigold";
}

void Marigold::deserialize(const std::string& data) {
    (void)data;
}

std::string Marigold::typeName() const { return "Marigold"; }
