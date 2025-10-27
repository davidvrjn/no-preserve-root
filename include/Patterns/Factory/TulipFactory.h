#pragma once
#include <memory>

#include "PlantFactory.h"

/**
 * @class TulipFactory
 * @brief A concrete factory that produces Tulip objects.
 */
class TulipFactory : public PlantFactory {
   public:
    TulipFactory();
    ~TulipFactory() override = default;

    std::shared_ptr<Plant> createPlant() override;
};
