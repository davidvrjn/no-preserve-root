#include "../../../include/Patterns/Factory/BasilFactory.h"

#include "../../../include/Components/Basil.h"

BasilFactory::BasilFactory()
{

}

std::shared_ptr<Plant> BasilFactory::createPlant() { return std::make_shared<Basil>(); }

double BasilFactory::getSeedCost() const 
{
    return 6.0;
}