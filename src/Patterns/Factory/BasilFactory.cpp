#include "../../../include/Patterns/Factory/BasilFactory.h"

#include "../../../include/Components/Basil.h"
#include "../../../include/Patterns/State/Seedling.h"

BasilFactory::BasilFactory() = default;

std::shared_ptr<Plant> BasilFactory::createPlant() {
    auto plant = std::make_shared<Basil>();
    plant->setState(std::make_unique<Seedling>());
    return plant;
}
