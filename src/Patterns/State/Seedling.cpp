#include "../../../include/Patterns/State/Seedling.h"

Seedling::Seedling() = default;

void Seedling::handleStateChange(Plant* plant) { (void)plant; }
void Seedling::performDailyActivity(Plant* plant) { (void)plant; }
std::unique_ptr<PlantState> Seedling::clone() const { return std::make_unique<Seedling>(); }
