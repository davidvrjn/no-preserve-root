#pragma once
#include <memory>

#include "PlantFactory.h"

/**
 * @class LavenderFactory
 * @brief A concrete factory that produces Lavender objects.
 */
class LavenderFactory : public PlantFactory {
   public:
    LavenderFactory();
    ~LavenderFactory() override = default;

    std::shared_ptr<Plant> createPlant() override;

    /**
     * @brief Returns the cost of a lavender seed
     * @return Cost in Rands
     */
    double getSeedCost() const override;
};
