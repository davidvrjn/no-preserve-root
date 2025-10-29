#include "../../../include/Patterns/Factory/SnakePlantFactory.h"

#include "../../../include/Components/SnakePlant.h"
#include "../../../include/Patterns/State/Seedling.h"

SnakePlantFactory::SnakePlantFactory() = default;

std::shared_ptr<Plant> SnakePlantFactory::createPlant() {
    auto plant = std::make_shared<SnakePlant>();
    plant->setState(std::make_unique<Seedling>());
    return plant;
}
