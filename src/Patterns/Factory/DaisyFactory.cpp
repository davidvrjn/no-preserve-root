#include "../../../include/Patterns/Factory/DaisyFactory.h"

#include "../../../include/Components/Daisy.h"
#include "../../../include/Patterns/State/Seedling.h"

DaisyFactory::DaisyFactory() = default;

double DaisyFactory::getSeedCost() const { return 7.0; }
std::shared_ptr<Plant> DaisyFactory::createPlant() {
    auto plant = std::make_shared<Daisy>();
    plant->setState(std::make_unique<Seedling>());
    plant->setWaterLevel(1 + plant->getWaterConsumption());
    return plant;
}
