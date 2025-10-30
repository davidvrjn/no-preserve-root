#include "../../../include/Patterns/Factory/RoseFactory.h"

#include "../../../include/Components/Rose.h"

RoseFactory::RoseFactory() = default;

std::shared_ptr<Plant> RoseFactory::createPlant() { return std::make_shared<Rose>(); }

double RoseFactory::getSeedCost() const 
{
    return 12.0;
}