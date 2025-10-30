#include "../../../include/Patterns/Factory/CactusFactory.h"

#include "../../../include/Components/Cactus.h"

CactusFactory::CactusFactory() = default;

std::shared_ptr<Plant> CactusFactory::createPlant() { return std::make_shared<Cactus>(); }

double CactusFactory::getSeedCost() const 
{
    return 7.0;
}