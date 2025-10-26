#include "../../../include/Patterns/Factory/IvyFactory.h"

#include "../../../include/Components/Ivy.h"

IvyFactory::IvyFactory() = default;

std::shared_ptr<Plant> IvyFactory::createPlant() { return std::make_shared<Ivy>(); }
