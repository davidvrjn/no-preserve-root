#include "../../../include/Patterns/State/Withering.h"

Withering::Withering() = default;

void Withering::handleStateChange(Plant* plant) { (void)plant; }
void Withering::performDailyActivity(Plant* plant) { (void)plant; }
std::unique_ptr<PlantState> Withering::clone() const { return std::make_unique<Withering>(); }

