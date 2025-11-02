#include "../../../include/Patterns/Factory/RoseFactory.h"

#include "../../../include/Components/Rose.h"
#include "../../../include/Patterns/State/Seedling.h"

RoseFactory::RoseFactory() = default;

double RoseFactory::getSeedCost() const 
{
    return 12.0;
}
std::shared_ptr<Plant> RoseFactory::createPlant() {
    auto plant = std::make_shared<Rose>();
    plant->setState(std::make_unique<Seedling>());
    plant->setWaterLevel(1 + plant->getWaterConsumption());
    return plant;
}
