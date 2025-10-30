#include "../../../include/Patterns/Factory/LavenderFactory.h"

#include "../../../include/Components/Lavender.h"
#include "../../../include/Patterns/State/Seedling.h"

LavenderFactory::LavenderFactory() = default;

double LavenderFactory::getSeedCost() const 
{
    return 11.0;
}
std::shared_ptr<Plant> LavenderFactory::createPlant() {
    auto plant = std::make_shared<Lavender>();
    plant->setState(std::make_unique<Seedling>());
    return plant;
}
