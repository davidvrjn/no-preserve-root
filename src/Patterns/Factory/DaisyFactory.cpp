#include "../../../include/Patterns/Factory/DaisyFactory.h"

#include "../../../include/Components/Daisy.h"

DaisyFactory::DaisyFactory() = default;

std::shared_ptr<Plant> DaisyFactory::createPlant() { return std::make_shared<Daisy>(); }

double DaisyFactory::getSeedCost() const 
{
    return 7.0;
}