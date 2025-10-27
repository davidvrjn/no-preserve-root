#pragma once
#include <memory>

#include "PlantFactory.h"

/**
 * @class BasilFactory
 * @brief A concrete factory that produces Basil objects.
 */
class BasilFactory : public PlantFactory {
   public:
    BasilFactory();
    ~BasilFactory() override = default;

    std::shared_ptr<Plant> createPlant() override;
};
