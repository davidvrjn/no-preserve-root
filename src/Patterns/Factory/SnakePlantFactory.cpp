#include "../../../include/Patterns/Factory/SnakePlantFactory.h"

#include "../../../include/Components/SnakePlant.h"

SnakePlantFactory::SnakePlantFactory() = default;

std::shared_ptr<Plant> SnakePlantFactory::createPlant() { return std::make_shared<SnakePlant>(); }
