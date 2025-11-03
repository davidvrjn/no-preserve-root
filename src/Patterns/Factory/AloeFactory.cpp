#include "../../../include/Patterns/Factory/AloeFactory.h"

#include "../../../include/Components/Aloe.h"
#include "../../../include/Patterns/State/Seedling.h"

AloeFactory::AloeFactory() = default;

double AloeFactory::getSeedCost() const { return 8.0; }

std::shared_ptr<Plant> AloeFactory::createPlant() {
    auto plant = std::make_shared<Aloe>();
    plant->setState(std::make_unique<Seedling>());
    plant->setWaterLevel(1 + plant->getWaterConsumption());
    return plant;
}
