#include "../../../include/Patterns/Factory/OrchidFactory.h"

#include "../../../include/Components/Orchid.h"
#include "../../../include/Patterns/State/Seedling.h"

OrchidFactory::OrchidFactory() = default;

double OrchidFactory::getSeedCost() const { return 15.0; }
std::shared_ptr<Plant> OrchidFactory::createPlant() {
    auto plant = std::make_shared<Orchid>();
    plant->setState(std::make_unique<Seedling>());
    plant->setWaterLevel(1 + plant->getWaterConsumption());
    return plant;
}
