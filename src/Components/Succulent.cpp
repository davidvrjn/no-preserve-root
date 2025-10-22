#include "../../include/Components/Succulent.h"

#include "../../include/Patterns/State/PlantState.h"

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
    return "Succulent";
}

void Succulent::deserialize(const std::string& data) {
    (void)data;
}

std::string Succulent::typeName() const { return "Succulent"; }
