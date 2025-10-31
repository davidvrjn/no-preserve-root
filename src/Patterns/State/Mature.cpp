#include "../../../include/Patterns/State/Mature.h"

Mature::Mature() = default;

//Stubbed, mature plants should be in stasis
void Mature::handleStateChange(Plant* plant) {
}

//Stubbed
void Mature::performDailyActivity(Plant* plant) {
}
std::unique_ptr<PlantState> Mature::clone() const { return std::make_unique<Mature>(); }
