#pragma once
#include <memory>

#include "PlantFactory.h"

/**
 * @class OrchidFactory
 * @brief A concrete factory that produces Orchid objects.
 */
class OrchidFactory : public PlantFactory {
   public:
    OrchidFactory();
    ~OrchidFactory() override = default;

    std::shared_ptr<Plant> createPlant() override;
    
    /**
     * @brief Returns the cost of an orchid seed
     * @return Cost in Rands
     */
    double getSeedCost() const override;
};
