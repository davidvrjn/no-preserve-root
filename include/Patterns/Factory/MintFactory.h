#pragma once
#include <memory>

#include "PlantFactory.h"

/**
 * @class MintFactory
 * @brief A concrete factory that produces Mint objects.
 */
class MintFactory : public PlantFactory {
   public:
    MintFactory();
    ~MintFactory() override = default;

    std::shared_ptr<Plant> createPlant() override;

    /**
     * @brief Returns the cost of a mint seed
     * @return Cost in Rands
     */
    double getSeedCost() const override;
};
