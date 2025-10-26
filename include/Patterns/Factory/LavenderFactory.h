#pragma once
#include <memory>

#include "PlantFactory.h"

/**
 * @class LavenderFactory
 * @brief A concrete factory that produces Lavender objects.
 */
class LavenderFactory : public PlantFactory {
   public:
    LavenderFactory();
    ~LavenderFactory() override = default;

    std::shared_ptr<Plant> createPlant() override;
};
