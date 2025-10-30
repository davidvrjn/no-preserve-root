#include "../../../include/Patterns/Factory/RoseFactory.h"

#include "../../../include/Components/Rose.h"
#include "../../../include/Patterns/State/Seedling.h"

RoseFactory::RoseFactory() = default;

double RoseFactory::getSeedCost() const 
{
    return 12.0;
}
std::shared_ptr<Plant> RoseFactory::createPlant() {
    auto rose = std::make_shared<Rose>();
    rose->setState(std::make_unique<Seedling>());
    return rose;
}
