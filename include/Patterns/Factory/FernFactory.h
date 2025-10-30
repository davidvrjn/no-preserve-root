#pragma once
#include <memory>

#include "PlantFactory.h"

/**
 * @class FernFactory
 * @brief A concrete factory that produces Fern objects.
 */
class FernFactory : public PlantFactory {
   public:
    FernFactory();
    ~FernFactory() override = default;

    std::shared_ptr<Plant> createPlant() override;
    
    /**
     * @brief Returns the cost of a fern seed
     * @return Cost in Rands
     */
    double getSeedCost() const override;
};
