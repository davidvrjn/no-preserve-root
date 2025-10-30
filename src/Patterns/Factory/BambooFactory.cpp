#include "../../../include/Patterns/Factory/BambooFactory.h"

#include "../../../include/Components/Bamboo.h"
#include "../../../include/Patterns/State/Seedling.h"

BambooFactory::BambooFactory() = default;

double BambooFactory::getSeedCost() const 
{
    return 10.0;
}
std::shared_ptr<Plant> BambooFactory::createPlant() {
    auto plant = std::make_shared<Bamboo>();
    plant->setState(std::make_unique<Seedling>());
    return plant;
}
