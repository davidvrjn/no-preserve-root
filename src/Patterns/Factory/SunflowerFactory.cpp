#include "../../../include/Patterns/Factory/SunflowerFactory.h"

#include "../../../include/Components/Sunflower.h"
#include "../../../include/Patterns/State/Seedling.h"

SunflowerFactory::SunflowerFactory() = default;

std::shared_ptr<Plant> SunflowerFactory::createPlant() {
    auto plant = std::make_shared<Sunflower>();
    plant->setState(std::make_unique<Seedling>());
    return plant;
}
