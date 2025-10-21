#include "../../../include/Patterns/Builder/ConcretePlantSpecificationBuilder.h"

ConcretePlantSpecificationBuilder::ConcretePlantSpecificationBuilder() = default;

// Provide the default constructor for PlantSpecification used by reset()/getResult().
PlantSpecification::PlantSpecification() : waterReq(LOW), sunReq(PARTIAL), requestType(RECOMMENDATION), explicitName() {}

void ConcretePlantSpecificationBuilder::setWaterRequirement(WaterLevel level) { (void)level; }
void ConcretePlantSpecificationBuilder::setSunRequirement(SunLevel level) { (void)level; }
void ConcretePlantSpecificationBuilder::addDecorator(const std::string& decorator) { (void)decorator; }
void ConcretePlantSpecificationBuilder::setRequestType(RequestType type) { (void)type; }
void ConcretePlantSpecificationBuilder::setExplicitName(const std::string& name) { (void)name; }
PlantSpecification ConcretePlantSpecificationBuilder::getResult() { return specification; }
void ConcretePlantSpecificationBuilder::reset() { specification = PlantSpecification(); }

