#include "../../../include/Patterns/Factory/FernFactory.h"

#include "../../../include/Components/Fern.h"

FernFactory::FernFactory() = default;

std::shared_ptr<Plant> FernFactory::createPlant() { return std::make_shared<Fern>(); }
