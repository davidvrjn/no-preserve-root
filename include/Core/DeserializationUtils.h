#pragma once

#include <memory>
#include <stdexcept>
#include <string>

#include "../Components/PlantAttributes.h"

// Forward declarations
class PlantState;
class Seedling;
class Growing;
class Mature;
class Withering;
class Withered;

/**
 * @brief Utility functions for deserializing enums and creating objects from strings
 */
namespace DeserializationUtils {

/**
 * @brief Parse WaterRequirement from string representation
 * @param str String representation ("VERY_LOW", "LOW", "MEDIUM", "HIGH")
 * @return WaterRequirement enum value
 * @throws std::invalid_argument if string is not recognized
 */
inline WaterRequirement parseWaterRequirement(const std::string& str) {
    if (str == "VERY_LOW") return WaterRequirement::VERY_LOW;
    if (str == "LOW") return WaterRequirement::LOW;
    if (str == "MEDIUM") return WaterRequirement::MEDIUM;
    if (str == "HIGH") return WaterRequirement::HIGH;
    throw std::invalid_argument("Unknown WaterRequirement: " + str);
}

/**
 * @brief Parse Season from string representation
 * @param str String representation ("SPRING", "SUMMER", "FALL", "WINTER", "YEAR_ROUND")
 * @return Season enum value
 * @throws std::invalid_argument if string is not recognized
 */
inline Season parseSeason(const std::string& str) {
    if (str == "SPRING") return Season::SPRING;
    if (str == "SUMMER") return Season::SUMMER;
    if (str == "FALL") return Season::FALL;
    if (str == "WINTER") return Season::WINTER;
    if (str == "YEAR_ROUND") return Season::YEAR_ROUND;
    throw std::invalid_argument("Unknown Season: " + str);
}

/**
 * @brief Create a PlantState from type string
 * @param type State type ("Seedling", "Growing", "Mature", "Withered", "Withering")
 * @param previousStateType For Withering state, the type of the previous state
 * @return unique_ptr to newly created PlantState
 * @throws std::invalid_argument if type is not recognized
 */
std::unique_ptr<PlantState> createState(const std::string& type,
                                       const std::string& previousStateType = "");

}  // namespace DeserializationUtils
