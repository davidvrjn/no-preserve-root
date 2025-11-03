#pragma once
#include <memory>

#include "PlantFactory.h"

/**
 * @class IvyFactory
 * @brief A concrete factory that produces Ivy objects.
 */
class IvyFactory : public PlantFactory {
   public:
    IvyFactory();
    ~IvyFactory() override = default;

    std::shared_ptr<Plant> createPlant() override;

    /**
     * @brief Returns the cost of an ivy seed
     * @return Cost in Rands
     */
    double getSeedCost() const override;
};
