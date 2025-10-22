#include "../../../include/Patterns/State/Withered.h"

Withered::Withered() = default;

void Withered::handleStateChange(Plant* plant) { (void)plant; }
void Withered::performDailyActivity(Plant* plant) { (void)plant; }
std::unique_ptr<PlantState> Withered::clone() const { return std::make_unique<Withered>(); }
