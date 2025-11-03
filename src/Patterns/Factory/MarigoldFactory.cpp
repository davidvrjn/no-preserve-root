#include "../../../include/Patterns/Factory/MarigoldFactory.h"

#include "../../../include/Components/Marigold.h"
#include "../../../include/Patterns/State/Seedling.h"

MarigoldFactory::MarigoldFactory() = default;

double MarigoldFactory::getSeedCost() const { return 6.0; }
std::shared_ptr<Plant> MarigoldFactory::createPlant() {
    auto plant = std::make_shared<Marigold>();
    plant->setState(std::make_unique<Seedling>());
    plant->setWaterLevel(1 + plant->getWaterConsumption());
    return plant;
}
