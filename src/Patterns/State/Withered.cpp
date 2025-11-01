#include "../../../include/Patterns/State/Withered.h"

Withered::Withered() = default;

// Plant is irrecoverable, stubbed
void Withered::handleStateChange(Plant* plant) { (void)plant; }

// Withered plants are dead but should notify observers
// so the supervisor can create a removeWitheredPlantCommand
void Withered::performDailyActivity(Plant* plant) {
    // Withered plants don't change state but notify observers
    // so they can be removed from the plot
    plant->notify();
}
std::unique_ptr<PlantState> Withered::clone() const { return std::make_unique<Withered>(); }
