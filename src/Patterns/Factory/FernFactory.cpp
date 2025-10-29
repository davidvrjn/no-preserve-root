#include "../../../include/Patterns/Factory/FernFactory.h"

#include "../../../include/Components/Fern.h"
#include "../../../include/Patterns/State/Seedling.h"

FernFactory::FernFactory() = default;

std::shared_ptr<Plant> FernFactory::createPlant() {
    auto plant = std::make_shared<Fern>();
    plant->setState(std::make_unique<Seedling>());
    return plant;
}
