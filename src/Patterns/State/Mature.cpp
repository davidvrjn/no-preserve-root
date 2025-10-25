#include "../../../include/Patterns/State/Mature.h"

Mature::Mature() = default;

void Mature::handleStateChange(Plant* plant) {
    if(plant->getHealth() <= 0){
        plant->setState(std::make_unique<Withering>(std::make_unique<Mature>()));
    }
}
void Mature::performDailyActivity(Plant* plant) {
    plant->setWaterLevel(plant->getWaterLevel() - plant->getWaterConsumption());
    //Should mature plants still be cared for.
    if(plant->getWaterLevel() <= 0){
        plant->setHealth(plant->getHealth() - 40); //Amount can be adjusted, but it takes 2 days of fertilising to recover an uncared plant fully
    }
    //Price functionality?
    handleStateChange(plant);
}
std::unique_ptr<PlantState> Mature::clone() const { return std::make_unique<Mature>(); }
