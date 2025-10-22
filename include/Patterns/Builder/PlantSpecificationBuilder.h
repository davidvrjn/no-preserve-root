#pragma once
#include "PlantSpecification.h"

/**
 * @interface PlantSpecificationBuilder
 * @brief The "Builder" interface for the Builder design pattern.
 *
 * Defines the contract for constructing a PlantSpecification object. It provides
 * a step-by-step interface for setting the various attributes of the specification.
 * This separates the complex construction logic from the final PlantSpecification data object.
 */
class PlantSpecificationBuilder {
   public:
    virtual ~PlantSpecificationBuilder() {}
    virtual void setWaterRequirement(WaterLevel level) = 0;
    virtual void setSunRequirement(SunLevel level) = 0;
    virtual void addDecorator(const std::string& decorator) = 0;
    virtual void setRequestType(RequestType type) = 0;
    virtual void setExplicitName(const std::string& name) = 0;
    virtual PlantSpecification getResult() = 0;
    virtual void reset() = 0;  // Method to clear the builder for reuse.
};
