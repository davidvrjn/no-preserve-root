#include "../../../include/Patterns/Factory/MarigoldFactory.h"

#include "../../../include/Components/Marigold.h"
#include "../../../include/Patterns/State/Seedling.h"

MarigoldFactory::MarigoldFactory() = default;

std::shared_ptr<Plant> MarigoldFactory::createPlant() {
    auto plant = std::make_shared<Marigold>();
    plant->setState(std::make_unique<Seedling>());
    return plant;
}
