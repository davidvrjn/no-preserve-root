#include "../../../include/Patterns/Factory/SucculentFactory.h"

#include "../../../include/Components/Succulent.h"
#include "../../../include/Patterns/State/Seedling.h"

SucculentFactory::SucculentFactory() = default;

double SucculentFactory::getSeedCost() const 
{
    return 7.0;
}
std::shared_ptr<Plant> SucculentFactory::createPlant() {
    auto plant = std::make_shared<Succulent>();
    plant->setState(std::make_unique<Seedling>());
    return plant;
}
