#include "../../../include/Patterns/Factory/SucculentFactory.h"

#include "../../../include/Components/Succulent.h"

SucculentFactory::SucculentFactory() = default;

std::shared_ptr<Plant> SucculentFactory::createPlant() { return std::make_shared<Succulent>(); }
