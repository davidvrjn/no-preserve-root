#pragma once
#include <memory>
#include "PlantFactory.h"


/**
 * @class AloeFactory
 * @brief A concrete factory that produces Aloe objects.
 */
class AloeFactory : public PlantFactory {
   public:
    AloeFactory();
    ~AloeFactory() override = default;

    std::shared_ptr<Plant> createPlant() override;
};
