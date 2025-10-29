#include "../../../include/Patterns/Factory/AloeFactory.h"

#include "../../../include/Components/Aloe.h"
#include "../../../include/Patterns/State/Seedling.h"

AloeFactory::AloeFactory() = default;

std::shared_ptr<Plant> AloeFactory::createPlant() {
    auto plant = std::make_shared<Aloe>();
    plant->setState(std::make_unique<Seedling>());
    return plant;
}
