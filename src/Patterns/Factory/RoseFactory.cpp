#include "../../../include/Patterns/Factory/RoseFactory.h"

#include "../../../include/Components/Rose.h"

RoseFactory::RoseFactory() = default;

std::shared_ptr<Plant> RoseFactory::createPlant() { return nullptr; }
