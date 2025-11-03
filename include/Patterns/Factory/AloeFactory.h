#pragma once
#include <memory>

#include "PlantFactory.h"

/**
 * @class AloeFactory
 * @brief A concrete factory that produces Aloe objects.
 */
class AloeFactory : public PlantFactory {
   public:
    AloeFactory();
    ~AloeFactory() override = default;

    std::shared_ptr<Plant> createPlant() override;

    /**
     * @brief Returns the cost of an aloe seed
     * @return Cost in Rands
     */
    double getSeedCost() const override;
};
