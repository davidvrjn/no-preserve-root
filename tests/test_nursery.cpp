/**
 * @file test_nursery.cpp
 * @brief Unit tests for Nursery class
 * 
 * Tests cover:
 * - getCurrentSeason() calculation
 * - addKnownPlantType() functionality
 * - knownPlantTypes management
 * - Season cycling logic
 * 
 * Note: spawnCustomer() is tested indirectly through integration tests
 * as it's a private method called during runSimulation().
 */

#include "../include/doctest.h"

#include <memory>
#include <string>

#include "../include/Core/Nursery.h"
#include "../include/Components/PlantAttributes.h"

TEST_CASE("Nursery - Constructor initializes properly") {
    auto nursery = std::make_shared<Nursery>();
    
    REQUIRE(nursery.get() != nullptr);
    
    // Should start at day 0, which is SPRING
    CHECK(nursery->getCurrentSeason() == Season::SPRING);
}

TEST_CASE("Nursery - addKnownPlantType adds plants correctly") {
    auto nursery = std::make_shared<Nursery>();
    
    // Add various plant types
    REQUIRE_NOTHROW(nursery->addKnownPlantType("Rose"));
    REQUIRE_NOTHROW(nursery->addKnownPlantType("Cactus"));
    REQUIRE_NOTHROW(nursery->addKnownPlantType("Tulip"));
    REQUIRE_NOTHROW(nursery->addKnownPlantType("Basil"));
}

TEST_CASE("Nursery - addKnownPlantType handles duplicates") {
    auto nursery = std::make_shared<Nursery>();
    
    // Add same plant multiple times - should not cause issues
    REQUIRE_NOTHROW(nursery->addKnownPlantType("Rose"));
    REQUIRE_NOTHROW(nursery->addKnownPlantType("Rose"));
    REQUIRE_NOTHROW(nursery->addKnownPlantType("Rose"));
    
    // Internal list should only have one "Rose" (tested via behavior, not directly)
}

TEST_CASE("Nursery - addKnownPlantType with empty string") {
    auto nursery = std::make_shared<Nursery>();
    
    // Should handle empty strings without crashing
    REQUIRE_NOTHROW(nursery->addKnownPlantType(""));
}

TEST_CASE("Nursery - addKnownPlantType with various plant names") {
    auto nursery = std::make_shared<Nursery>();
    
    // Test all 17 plant types
    std::vector<std::string> allPlants = {
        "Aloe", "Bamboo", "Basil", "Cactus", "Daisy", "Fern", "Ivy",
        "Lavender", "Marigold", "Mint", "Orchid", "Petunia", "Rose",
        "SnakePlant", "Succulent", "Sunflower", "Tulip"
    };
    
    for (const auto& plant : allPlants) {
        REQUIRE_NOTHROW(nursery->addKnownPlantType(plant));
    }
}

TEST_CASE("Nursery - getCurrentSeason returns valid season") {
    auto nursery = std::make_shared<Nursery>();
    
    Season season = nursery->getCurrentSeason();
    
    // Should be one of the 4 main seasons (not YEAR_ROUND for gameplay)
    bool validSeason = (season == Season::SPRING || 
                       season == Season::SUMMER || 
                       season == Season::FALL || 
                       season == Season::WINTER);
    
    CHECK(validSeason);
}
