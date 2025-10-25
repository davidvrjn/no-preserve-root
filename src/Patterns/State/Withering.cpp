#include "../../../include/Patterns/State/Withering.h"

Withering::Withering(std::unique_ptr<PlantState> prev){
    previousState = std::move(prev);
}

void Withering::handleStateChange(Plant* plant) {
    if(plant->getHealth() > 0){
        //Override the health gained from fertilising to always go back to 20 hp if recovering
        plant->setHealth(20);
        plant->setState(std::move(previousState));
    }
    //2 days have passed without care
    else if(plant->getHealth() < -2){
        plant->setState(std::make_unique<Withered>());
    }
}
void Withering::performDailyActivity(Plant* plant) {
    plant->setHealth(plant->getHealth() - 1);
    //Price decreasing logic should probably be added here
    handleStateChange(plant);
}
std::unique_ptr<PlantState> Withering::clone() const { return std::make_unique<Withering>(); }
