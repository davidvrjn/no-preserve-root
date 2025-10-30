#pragma once
#include <memory>

#include "PlantFactory.h"

/**
 * @class SnakePlantFactory
 * @brief A concrete factory that produces SnakePlant objects.
 */
class SnakePlantFactory : public PlantFactory {
   public:
    SnakePlantFactory();
    ~SnakePlantFactory() override = default;

    std::shared_ptr<Plant> createPlant() override;
    
    /**
     * @brief Returns the cost of a snake plant seed
     * @return Cost in Rands
     */
    double getSeedCost() const override;
};
