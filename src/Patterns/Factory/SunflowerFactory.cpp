#include "../../../include/Patterns/Factory/SunflowerFactory.h"

#include "../../../include/Components/Sunflower.h"

SunflowerFactory::SunflowerFactory() = default;

std::shared_ptr<Plant> SunflowerFactory::createPlant() { return std::make_shared<Sunflower>(); }

double SunflowerFactory::getSeedCost() const 
{
    return 8.0;
}