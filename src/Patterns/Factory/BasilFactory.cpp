#include "../../../include/Patterns/Factory/BasilFactory.h"

#include "../../../include/Components/Basil.h"

BasilFactory::BasilFactory() = default;

std::shared_ptr<Plant> BasilFactory::createPlant() { return std::make_shared<Basil>(); }
