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
    
    /**
     * @brief Returns the cost of a tulip seed
     * @return Cost in Rands
     */
    double getSeedCost() const override;
};
