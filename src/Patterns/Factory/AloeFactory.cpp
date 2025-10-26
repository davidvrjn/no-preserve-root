#include "../../../include/Patterns/Factory/AloeFactory.h"

#include "../../../include/Components/Aloe.h"

AloeFactory::AloeFactory() = default;

std::shared_ptr<Plant> AloeFactory::createPlant() { return std::make_shared<Aloe>(); }
