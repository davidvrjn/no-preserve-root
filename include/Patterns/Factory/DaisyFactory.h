#pragma once
#include <memory>

#include "PlantFactory.h"

/**
 * @class DaisyFactory
 * @brief A concrete factory that produces Daisy objects.
 */
class DaisyFactory : public PlantFactory {
   public:
    DaisyFactory();
    ~DaisyFactory() override = default;

    std::shared_ptr<Plant> createPlant() override;
};
