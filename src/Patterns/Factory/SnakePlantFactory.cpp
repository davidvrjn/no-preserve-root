#include "../../../include/Patterns/Factory/SnakePlantFactory.h"

#include "../../../include/Components/SnakePlant.h"
#include "../../../include/Patterns/State/Seedling.h"

SnakePlantFactory::SnakePlantFactory() = default;

double SnakePlantFactory::getSeedCost() const { return 9.0; }
std::shared_ptr<Plant> SnakePlantFactory::createPlant() {
    auto plant = std::make_shared<SnakePlant>();
    plant->setState(std::make_unique<Seedling>());
    plant->setWaterLevel(1 + plant->getWaterConsumption());
    return plant;
}
