#include "../../../include/Patterns/Factory/CactusFactory.h"

#include "../../../include/Components/Cactus.h"
#include "../../../include/Patterns/State/Seedling.h"

CactusFactory::CactusFactory() = default;

std::shared_ptr<Plant> CactusFactory::createPlant() {
    auto cactus = std::make_shared<Cactus>();
    // All newly planted plants start in Seedling state
    cactus->setState(std::make_unique<Seedling>());
    return cactus;
}
