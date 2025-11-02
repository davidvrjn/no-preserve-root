#include "../../../include/Patterns/Factory/CactusFactory.h"

#include "../../../include/Components/Cactus.h"
#include "../../../include/Patterns/State/Seedling.h"

CactusFactory::CactusFactory()  = default;

double CactusFactory::getSeedCost() const 
{
    return 7.0;
}
std::shared_ptr<Plant> CactusFactory::createPlant() {
    auto plant = std::make_shared<Cactus>();
    plant->setState(std::make_unique<Seedling>());
    plant->setWaterLevel(1 + plant->getWaterConsumption());
    return plant;
}
