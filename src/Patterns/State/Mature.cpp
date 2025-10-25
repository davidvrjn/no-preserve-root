#include "../../../include/Patterns/State/Mature.h"

Mature::Mature() = default;

void Mature::handleStateChange(Plant* plant) { (void)plant; }
void Mature::performDailyActivity(Plant* plant) {
    plant->setWaterLevel(plant->getWaterLevel() - plant->getWaterConsumption());
    

}
std::unique_ptr<PlantState> Mature::clone() const { return std::make_unique<Mature>(); }
