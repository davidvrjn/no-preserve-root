#pragma once
#include <memory>

#include "PlantFactory.h"

/**
 * @class CactusFactory
 * @brief A concrete factory that produces Cactus objects.
 */
class CactusFactory : public PlantFactory {
   public:
    CactusFactory();
    ~CactusFactory() override = default;

    std::shared_ptr<Plant> createPlant() override;
    
    /**
     * @brief Returns the cost of a cactus seed
     * @return Cost in Rands
     */
    double getSeedCost() const override;
};
