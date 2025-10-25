#include "../../../include/Patterns/State/Seedling.h"

Seedling::Seedling() = default;

void Seedling::handleStateChange(Plant* plant) {
    if(plant->getHealth() <= 0){
        //Should age be reversed back to today, or can it still progress today
        plant->setState(std::make_unique<Withering>(std::make_unique<Seedling>()));
    }
    //Switch state to growing
    else if(plant->getAge() >= plant->getSeedlingDuration()){
        plant->setState(std::make_unique<Growing>());
    }
}

void Seedling::performDailyActivity(Plant* plant) { 
    plant->setWaterLevel(plant->getWaterLevel() - plant->getWaterConsumption());
    //Plant is not watered, loose health
    if(plant->getWaterLevel() <= 0){
        plant->setHealth(plant->getHealth() - 40); //Amount can be adjusted, but it takes 2 days of fertilising to recover an uncared plant fully
    }
    plant->setAge(plant->getAge() + 1);
    //Implement price increase with season, maybe a health penalty for being in the incorrect season.
    handleStateChange(plant);

}
std::unique_ptr<PlantState> Seedling::clone() const { return std::make_unique<Seedling>(); }
