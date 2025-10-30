#include "../../../include/Patterns/Factory/CactusFactory.h"

#include "../../../include/Components/Cactus.h"
#include "../../../include/Patterns/State/Seedling.h"

CactusFactory::CactusFactory()  = default;

double CactusFactory::getSeedCost() const 
{
    return 7.0;
}
std::shared_ptr<Plant> CactusFactory::createPlant() {
    auto cactus = std::make_shared<Cactus>();
    cactus->setState(std::make_unique<Seedling>());
    return cactus;
}
