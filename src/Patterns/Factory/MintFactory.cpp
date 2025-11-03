#include "../../../include/Patterns/Factory/MintFactory.h"

#include "../../../include/Components/Mint.h"
#include "../../../include/Patterns/State/Seedling.h"

MintFactory::MintFactory() = default;

double MintFactory::getSeedCost() const { return 5.0; }
std::shared_ptr<Plant> MintFactory::createPlant() {
    auto plant = std::make_shared<Mint>();
    plant->setState(std::make_unique<Seedling>());
    plant->setWaterLevel(1 + plant->getWaterConsumption());
    return plant;
}
