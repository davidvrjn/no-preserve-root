#include "../../../include/Patterns/State/Withered.h"

Withered::Withered() = default;

//Plant is irrecoverable, stubbed
void Withered::handleStateChange(Plant* plant) { (void)plant; }
//Stubbed, unless price logic should be added, but it should probably just be 0
void Withered::performDailyActivity(Plant* plant) { (void)plant; }
std::unique_ptr<PlantState> Withered::clone() const { return std::make_unique<Withered>(); }
