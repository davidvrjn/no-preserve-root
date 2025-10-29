#include "../../../include/Patterns/Factory/MintFactory.h"

#include "../../../include/Components/Mint.h"
#include "../../../include/Patterns/State/Seedling.h"

MintFactory::MintFactory() = default;

std::shared_ptr<Plant> MintFactory::createPlant() {
    auto plant = std::make_shared<Mint>();
    plant->setState(std::make_unique<Seedling>());
    return plant;
}
