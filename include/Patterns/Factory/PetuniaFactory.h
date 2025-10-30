#pragma once
#include <memory>

#include "PlantFactory.h"

/**
 * @class PetuniaFactory
 * @brief A concrete factory that produces Petunia objects.
 */
class PetuniaFactory : public PlantFactory {
   public:
    PetuniaFactory();
    ~PetuniaFactory() override = default;

    std::shared_ptr<Plant> createPlant() override;
    
    /**
     * @brief Returns the cost of a petunia seed
     * @return Cost in Rands
     */
    double getSeedCost() const override;
};
