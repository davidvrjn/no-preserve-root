#pragma once
#include <string>
#include <vector>

// Forward declare enums from PlantAttributes.h to avoid circular dependency
enum class WaterRequirement;
enum class Season;

enum RequestType { RECOMMENDATION, PURCHASE };

/**
 * @struct PlantSpecification
 * @brief The "Product" in the Builder pattern.
 *
 * This is a simple data structure that holds all the attributes of a customer's
 * request. It is constructed piece by piece by the PlantSpecificationBuilder.
 * It contains all the information a Cashier needs to find a plant and complete a transaction.
 *
 * For RECOMMENDATION requests:
 *   - Uses waterReq and seasonReq to match plant attributes
 *   - explicitName is empty
 *   - decorators is empty (can't decorate a recommendation)
 *
 * For PURCHASE requests:
 *   - Uses explicitName to specify the exact plant
 *   - waterReq and seasonReq are ignored
 *   - decorators may be populated (gift wrap, pot, ribbon)
 */
struct PlantSpecification {
    // For RECOMMENDATION - describe what customer wants
    WaterRequirement waterReq;
    Season seasonReq;
    
    // For PURCHASE - explicit plant name
    std::string explicitName;
    
    // Decorators (only valid for PURCHASE requests)
    std::vector<std::string> decorators;
    
    RequestType requestType;

    PlantSpecification();
};
