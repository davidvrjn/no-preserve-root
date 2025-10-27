#include "../../../include/Patterns/State/Withering.h"

Withering::Withering(std::unique_ptr<PlantState> prev) { previousState = std::move(prev); }

void Withering::handleStateChange(Plant* plant) {
    // Recovery path: only happens when fertilize() is explicitly called
    if (plant->getHealth() > 0 && previousState) {
        // Restore to previous state (Seedling or Growing)
        plant->setState(std::move(previousState));
    }
    // Death path: 2+ days without care
    else if (plant->getHealth() < -2) {
        plant->setState(std::make_unique<Withered>());
    }
}
void Withering::performDailyActivity(Plant* plant) {
    plant->setHealth(plant->getHealth() - 1);
    // Price decreasing logic should probably be added here
    handleStateChange(plant);
}
std::unique_ptr<PlantState> Withering::clone() const {
    return std::make_unique<Withering>(previousState ? previousState->clone() : nullptr);
}
