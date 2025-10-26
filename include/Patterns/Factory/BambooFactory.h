#pragma once
#include <memory>

#include "PlantFactory.h"

/**
 * @class BambooFactory
 * @brief A concrete factory that produces Bamboo objects.
 */
class BambooFactory : public PlantFactory {
   public:
    BambooFactory();
    ~BambooFactory() override = default;

    std::shared_ptr<Plant> createPlant() override;
};
