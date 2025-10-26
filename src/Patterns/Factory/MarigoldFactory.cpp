#include "../../../include/Patterns/Factory/MarigoldFactory.h"

#include "../../../include/Components/Marigold.h"

MarigoldFactory::MarigoldFactory() = default;

std::shared_ptr<Plant> MarigoldFactory::createPlant() { return std::make_shared<Marigold>(); }
