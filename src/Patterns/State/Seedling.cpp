#include "../../../include/Patterns/State/Seedling.h"

Seedling::Seedling() = default;

void Seedling::handleStateChange(Plant* plant) {
    //Switch state to growing
    if(plant->getAge() >= plant->getSeedlingDuration()){
        plant->setState(std::make_unique<Growing>());
    }
}

void Seedling::performDailyActivity(Plant* plant) { 
    plant->setWaterLevel(plant->getWaterLevel() - plant->getWaterConsumption());
    plant->setAge(plant->getAge() + 1);
    //Health decrease under which condtions
    handleStateChange(plant);

}
std::unique_ptr<PlantState> Seedling::clone() const { return std::make_unique<Seedling>(); }
