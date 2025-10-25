#include "../../../include/Patterns/State/Growing.h"

Growing::Growing() = default;

void Growing::handleStateChange(Plant* plant) {
    if(plant->getHealth() <= 0){
        //Should age be reversed back to today, or can it still progress today
        plant->setState(std::make_unique<Withering>(std::make_unique<Growing>()));
    }
    else if(plant->getAge() >= plant->getGrowingDuration()){
        plant->setState(std::make_unique<Mature>());
    }
}
void Growing::performDailyActivity(Plant* plant) {
    plant->setWaterLevel(plant->getWaterLevel() - plant->getWaterConsumption());
    //Plant is not watered, loose health
    if(plant->getWaterLevel() <= 0){
        plant->setHealth(plant->getHealth() - 40); //Amount can be adjusted, but it takes 2 days of fertilising to recover an uncared plant fully
    }
    plant->setAge(plant->getAge() + 1);

    //What effect does season have
    handleStateChange(plant);
}
std::unique_ptr<PlantState> Growing::clone() const { return std::make_unique<Growing>(); }
