#include "../../../include/Patterns/Builder/ConcretePlantSpecificationBuilder.h"


ConcretePlantSpecificationBuilder::ConcretePlantSpecificationBuilder() = default;

// Provide the default constructor for PlantSpecification used by reset()/getResult().
PlantSpecification::PlantSpecification()
    : waterReq(WaterRequirement::MEDIUM),
      seasonReq(Season::YEAR_ROUND),
      explicitName(),
      decorators(),
      requestType(RECOMMENDATION) {}


void ConcretePlantSpecificationBuilder::setWaterRequirement(WaterRequirement level) {
    specification.waterReq = level;
}

void ConcretePlantSpecificationBuilder::setSeasonRequirement(Season season) {
    specification.seasonReq = season;
}


void ConcretePlantSpecificationBuilder::addDecorator(const std::string& decorator) {
    // Only add decorators if this is a PURCHASE request
    if (specification.requestType == PURCHASE) {
        specification.decorators.push_back(decorator);
    }
}


void ConcretePlantSpecificationBuilder::setRequestType(RequestType type) {
    specification.requestType = type;
}

void ConcretePlantSpecificationBuilder::setExplicitName(const std::string& name) {
    specification.explicitName = name;
}

PlantSpecification ConcretePlantSpecificationBuilder::getResult() { return specification; }

void ConcretePlantSpecificationBuilder::reset() { specification = PlantSpecification(); }
