#include "../../../include/Patterns/Factory/AloeFactory.h"

#include "../../../include/Components/Aloe.h"

AloeFactory::AloeFactory() 
{
    
}

std::shared_ptr<Plant> AloeFactory::createPlant() { return std::make_shared<Aloe>(); }


double AloeFactory::getSeedCost() const 
{
    return 8.0;
}