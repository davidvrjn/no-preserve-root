#pragma once
#include <string>
#include <vector>

// Enums to define plant attributes clearly.
// These would likely be defined in a separate utility header.
enum WaterLevel { LOW, MEDIUM, HIGH };
enum SunLevel { SHADE, PARTIAL, FULL };
enum RequestType { RECOMMENDATION, PURCHASE };

/**
 * @struct PlantSpecification
 * @brief The "Product" in the Builder pattern.
 *
 * This is a simple data structure that holds all the attributes of a customer's
 * request. It is constructed piece by piece by the PlantSpecificationBuilder.
 * It contains all the information a Cashier needs to find a plant and complete a transaction.
 */
struct PlantSpecification {
    WaterLevel waterReq;
    SunLevel sunReq;
    std::vector<std::string> decorators;
    RequestType requestType;
    std::string explicitName;

    PlantSpecification();
};
