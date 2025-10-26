#include "../../../include/Patterns/Factory/BambooFactory.h"

#include "../../../include/Components/Bamboo.h"

BambooFactory::BambooFactory() = default;

std::shared_ptr<Plant> BambooFactory::createPlant() { return std::make_shared<Bamboo>(); }
