#pragma once

#include "../Patterns/Builder/PlantSpecification.h"

/**
 * @file PlantAttributes.h
 * @brief Defines common enums and constants for plant characteristics
 */

// Water requirement levels (daily consumption)
enum class WaterRequirement {
    VERY_LOW,   // 1-2 per day (cacti, succulents)
    LOW,        // 3-4 per day (lavender, herbs)
    MEDIUM,     // 5-6 per day (roses, common flowers)
    HIGH        // 7-8 per day (ferns, mint)
};

// Growing seasons
enum class Season {
    SPRING,
    SUMMER,
    FALL,
    WINTER,
    YEAR_ROUND  // No seasonal preference
};

// Helper function to convert WaterRequirement to daily consumption rate
inline int waterRequirementToConsumption(WaterRequirement req) {
    switch (req) {
        case WaterRequirement::VERY_LOW: return 2;  // Average of 1-2
        case WaterRequirement::LOW: return 4;       // Average of 3-4
        case WaterRequirement::MEDIUM: return 5;    // Average of 5-6
        case WaterRequirement::HIGH: return 7;      // Average of 7-8
        default: return 5;
    }
}

// Helper to convert to builder pattern enums (for customer requests)
inline WaterLevel waterRequirementToWaterLevel(WaterRequirement req) {
    switch (req) {
        case WaterRequirement::VERY_LOW:
        case WaterRequirement::LOW:
            return WaterLevel::LOW;
        case WaterRequirement::MEDIUM:
            return WaterLevel::MEDIUM;
        case WaterRequirement::HIGH:
            return WaterLevel::HIGH;
        default: return WaterLevel::MEDIUM;
    }
}
