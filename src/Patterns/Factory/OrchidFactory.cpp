#include "../../../include/Patterns/Factory/OrchidFactory.h"

#include "../../../include/Components/Orchid.h"

OrchidFactory::OrchidFactory() = default;

std::shared_ptr<Plant> OrchidFactory::createPlant() { return std::make_shared<Orchid>(); }

double OrchidFactory::getSeedCost() const 
{
    return 15.0;
}