#include "../../../include/Patterns/State/Mature.h"

Mature::Mature() = default;

//Stubbed, mature plants should be in stasis
void Mature::handleStateChange(Plant* plant) {
}

// Mature plants are in stasis but should notify observers
// so the supervisor can create an addToStorageCommand
void Mature::performDailyActivity(Plant* plant) {
    // Mature plants don't consume water or change state
    // But notify observers so they can be moved to storage
    plant->notify();
}
std::unique_ptr<PlantState> Mature::clone() const { return std::make_unique<Mature>(); }
