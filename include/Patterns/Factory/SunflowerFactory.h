#pragma once
#include <memory>

#include "PlantFactory.h"

/**
 * @class SunflowerFactory
 * @brief A concrete factory that produces Sunflower objects.
 */
class SunflowerFactory : public PlantFactory {
   public:
    SunflowerFactory();
    ~SunflowerFactory() override = default;

    std::shared_ptr<Plant> createPlant() override;
};
