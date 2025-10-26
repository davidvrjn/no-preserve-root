#pragma once
#include <memory>

#include "PlantFactory.h"

/**
 * @class MarigoldFactory
 * @brief A concrete factory that produces Marigold objects.
 */
class MarigoldFactory : public PlantFactory {
   public:
    MarigoldFactory();
    ~MarigoldFactory() override = default;

    std::shared_ptr<Plant> createPlant() override;
};
