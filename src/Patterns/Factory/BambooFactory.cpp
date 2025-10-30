#include "../../../include/Patterns/Factory/BambooFactory.h"

#include "../../../include/Components/Bamboo.h"

BambooFactory::BambooFactory() 
{
    
}

std::shared_ptr<Plant> BambooFactory::createPlant() { return std::make_shared<Bamboo>(); }

double BambooFactory::getSeedCost() const 
{
    return 10.0;
}