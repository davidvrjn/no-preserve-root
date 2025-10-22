#include "../../../include/Patterns/State/Growing.h"

Growing::Growing() = default;

void Growing::handleStateChange(Plant* plant) { (void)plant; }
void Growing::performDailyActivity(Plant* plant) { (void)plant; }
std::unique_ptr<PlantState> Growing::clone() const { return std::make_unique<Growing>(); }
