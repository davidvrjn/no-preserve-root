#include "../../../include/Patterns/Factory/TulipFactory.h"

#include "../../../include/Components/Tulip.h"
#include "../../../include/Patterns/State/Seedling.h"

TulipFactory::TulipFactory() = default;

std::shared_ptr<Plant> TulipFactory::createPlant() {
    auto plant = std::make_shared<Tulip>();
    plant->setState(std::make_unique<Seedling>());
    return plant;
}
