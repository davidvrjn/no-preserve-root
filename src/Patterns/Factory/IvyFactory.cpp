#include "../../../include/Patterns/Factory/IvyFactory.h"

#include "../../../include/Components/Ivy.h"
#include "../../../include/Patterns/State/Seedling.h"

IvyFactory::IvyFactory() = default;

double IvyFactory::getSeedCost() const 
{
    return 8.0;
}
std::shared_ptr<Plant> IvyFactory::createPlant() {
    auto plant = std::make_shared<Ivy>();
    plant->setState(std::make_unique<Seedling>());
    return plant;
}
