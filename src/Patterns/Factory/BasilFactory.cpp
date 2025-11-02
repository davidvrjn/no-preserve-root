#include "../../../include/Patterns/Factory/BasilFactory.h"

#include "../../../include/Components/Basil.h"
#include "../../../include/Patterns/State/Seedling.h"

BasilFactory::BasilFactory() = default;

double BasilFactory::getSeedCost() const { return 6.0; }
std::shared_ptr<Plant> BasilFactory::createPlant() {
    auto plant = std::make_shared<Basil>();
    plant->setState(std::make_unique<Seedling>());
    plant->setWaterLevel(1 + plant->getWaterConsumption());
    return plant;
}
