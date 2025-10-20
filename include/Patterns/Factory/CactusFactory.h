#pragma once
#include "PlantFactory.h"

/**
 * @class CactusFactory
 * @brief A concrete factory that produces Cactus objects.
 */
class CactusFactory : public PlantFactory {
public:
    CactusFactory();
    ~CactusFactory();
    
    Plant* createPlant() override;
};
