#pragma once
#include <memory>

#include "PlantFactory.h"

/**
 * @class SucculentFactory
 * @brief A concrete factory that produces Succulent objects.
 */
class SucculentFactory : public PlantFactory {
   public:
    SucculentFactory();
    ~SucculentFactory() override = default;

    std::shared_ptr<Plant> createPlant() override;
};
