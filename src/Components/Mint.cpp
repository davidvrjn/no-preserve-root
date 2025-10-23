#include "../../include/Components/Mint.h"

#include "../../include/Patterns/State/PlantState.h"

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
    return "Mint";
}

void Mint::deserialize(const std::string& data) {
    (void)data;
}

std::string Mint::typeName() const { return "Mint"; }
