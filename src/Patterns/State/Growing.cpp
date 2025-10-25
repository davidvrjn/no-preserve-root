#include "../../../include/Patterns/State/Growing.h"

Growing::Growing() = default;

void Growing::handleStateChange(Plant* plant) {
    if(plant->getAge() >= plant->getGrowingDuration()){
        plant->setState(std::make_unique<Mature>());
    }
}
void Growing::performDailyActivity(Plant* plant) {
    plant->setWaterLevel(plant->getWaterLevel() - plant->getWaterConsumption());
    plant->setAge(plant->getAge() + 1);
    //Health decrease under which condtions
    //What effect does season have
    handleStateChange(plant);
}
std::unique_ptr<PlantState> Growing::clone() const { return std::make_unique<Growing>(); }
