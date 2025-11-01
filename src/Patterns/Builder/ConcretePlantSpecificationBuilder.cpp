#include "../../../include/Patterns/Builder/ConcretePlantSpecificationBuilder.h"


/**
 * @brief Default constructor for ConcretePlantSpecificationBuilder.
 * 
 */
ConcretePlantSpecificationBuilder::ConcretePlantSpecificationBuilder() = default;

/**
 * @brief  Default constructor for PlantSpecification
 * Initializes a PlantSpecification with default values:
 * - Water requirement: MEDIUM
 * - Season requirement: YEAR_ROUND
 * - Empty explicit name
 * - Empty decorators list
 * - Request type: RECOMMENDATION
 */
PlantSpecification::PlantSpecification()
    : waterReq(WaterRequirement::MEDIUM),
      seasonReq(Season::YEAR_ROUND),
      explicitName(),
      decorators(),
      requestType(RECOMMENDATION) {}

/**
 * @brief Sets the water requirement for the plant specification.
 * 
 * @param level The water requirement level (e.g., LOW, MEDIUM, HIGH).
 */
void ConcretePlantSpecificationBuilder::setWaterRequirement(WaterRequirement level) {
    specification.waterReq = level;
}

/**
 * @brief Sets the season requirement for the plant specification.
 * 
 * @param season The season when the plant should be grown (e.g., SPRING, SUMMER, YEAR_ROUND).
 */
void ConcretePlantSpecificationBuilder::setSeasonRequirement(Season season) {
    specification.seasonReq = season;
}

/**
 * @brief Adds a decorator to the plant specification.
 * 
 * * Decorators are only added if the request type is PURCHASE.
 * This prevents decorators from being applied to RECOMMENDATION requests.
 * 
 * @param decorator The name of the decorator to add (e.g., "Fertilized", "Organic").
 */
void ConcretePlantSpecificationBuilder::addDecorator(const std::string& decorator) {
    // Only add decorators if this is a PURCHASE request
    if (specification.requestType == PURCHASE) {
        specification.decorators.push_back(decorator);
    }
}

/**
 * @brief Sets the request type for the plant specification.
 * 
 * @param type The type of request (RECOMMENDATION or PURCHASE).
 */
void ConcretePlantSpecificationBuilder::setRequestType(RequestType type) {
    specification.requestType = type;
}

/**
 * @brief Sets an explicit plant name for the specification.
 * 
 * Used when the customer wants a specific plant by name rather than
 * requesting a recommendation based on requirements.
 * 
 * @param name The explicit name of the desired plant.
 */
void ConcretePlantSpecificationBuilder::setExplicitName(const std::string& name) {
    specification.explicitName = name;
}

/**
 * @brief Returns the constructed PlantSpecification.
 * 
 * @return PlantSpecification The completed plant specification object.
 */
PlantSpecification ConcretePlantSpecificationBuilder::getResult() { return specification; }

/**
 * @brief Resets the builder to start constructing a new PlantSpecification.
 * 
 * Creates a fresh PlantSpecification with default values, allowing the builder
 * to be reused for creating multiple specifications.
 */
void ConcretePlantSpecificationBuilder::reset() { specification = PlantSpecification(); }
