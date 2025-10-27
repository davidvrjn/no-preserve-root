#pragma once
#include <memory>

#include "PlantFactory.h"

/**
 * @class IvyFactory
 * @brief A concrete factory that produces Ivy objects.
 */
class IvyFactory : public PlantFactory {
   public:
    IvyFactory();
    ~IvyFactory() override = default;

    std::shared_ptr<Plant> createPlant() override;
};
