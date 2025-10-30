#include "../../../include/Patterns/Factory/TulipFactory.h"

#include "../../../include/Components/Tulip.h"

TulipFactory::TulipFactory() = default;

std::shared_ptr<Plant> TulipFactory::createPlant() { return std::make_shared<Tulip>(); }

double TulipFactory::getSeedCost() const 
{
    return 10.0;
}