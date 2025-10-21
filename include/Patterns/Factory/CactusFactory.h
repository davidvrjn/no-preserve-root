#pragma once
#include "PlantFactory.h"
#include <memory>

/**
 * @class CactusFactory
 * @brief A concrete factory that produces Cactus objects.
 */
class CactusFactory : public PlantFactory {
public:
    CactusFactory();
    ~CactusFactory() override = default;
    
    std::shared_ptr<Plant> createPlant() override;
};
