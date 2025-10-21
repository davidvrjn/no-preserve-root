#pragma once
#include "PlantSpecificationBuilder.h"

/**
 * @class ConcretePlantSpecificationBuilder
 * @brief The "ConcreteBuilder" implementation.
 * 
 * This class implements the builder interface. It holds a private instance of the
 * PlantSpecification and builds it up as its methods are called. The getResult()
 * method returns the completed object.
 */
class ConcretePlantSpecificationBuilder : public PlantSpecificationBuilder {
private:
    PlantSpecification specification;

public:
    ConcretePlantSpecificationBuilder();
    ~ConcretePlantSpecificationBuilder();

    void setWaterRequirement(WaterLevel level) override;
    void setSunRequirement(SunLevel level) override;
    void addDecorator(const std::string& decorator) override;
    void setRequestType(RequestType type) override;
    void setExplicitName(const std::string& name) override;
    PlantSpecification getResult() override;
    void reset() override;
};
