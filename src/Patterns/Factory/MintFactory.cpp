#include "../../../include/Patterns/Factory/MintFactory.h"

#include "../../../include/Components/Mint.h"

MintFactory::MintFactory() = default;

std::shared_ptr<Plant> MintFactory::createPlant() { return std::make_shared<Mint>(); }

double MintFactory::getSeedCost() const 
{
    return 5.0;
}