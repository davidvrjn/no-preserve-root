#include "../../../include/Patterns/Factory/FernFactory.h"

#include "../../../include/Components/Fern.h"
#include "../../../include/Patterns/State/Seedling.h"

FernFactory::FernFactory() = default;

double FernFactory::getSeedCost() const { return 9.0; }
std::shared_ptr<Plant> FernFactory::createPlant() {
    auto plant = std::make_shared<Fern>();
    plant->setState(std::make_unique<Seedling>());
    plant->setWaterLevel(1 + plant->getWaterConsumption());
    return plant;
}
