
#pragma once
#include <memory>

#include "PlantFactory.h"

/**
 * @class RoseFactory
 * @brief A concrete factory that produces Rose objects.
 */
class RoseFactory : public PlantFactory {
   public:
    RoseFactory();
    ~RoseFactory() override = default;

    std::shared_ptr<Plant> createPlant() override;
};
