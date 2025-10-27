/**
 * @file test_builder.cpp
 * @brief Unit tests for Builder pattern implementation
 * 
 * Tests cover:
 * - ConcretePlantSpecificationBuilder functionality
 * - PlantSpecification construction
 * - RECOMMENDATION request specifications
 * - PURCHASE request specifications
 * - Decorator handling (only for PURCHASE)
 * - Builder reset functionality
 */

#include "../include/doctest.h"

#include <memory>
#include <string>

#include "../include/Patterns/Builder/ConcretePlantSpecificationBuilder.h"
#include "../include/Patterns/Builder/PlantSpecification.h"
#include "../include/Components/PlantAttributes.h"

TEST_CASE("PlantSpecification - Default constructor creates valid defaults") {
    PlantSpecification spec;
    
    CHECK(spec.waterReq == WaterRequirement::MEDIUM);
    CHECK(spec.seasonReq == Season::YEAR_ROUND);
    CHECK(spec.explicitName.empty());
    CHECK(spec.decorators.empty());
    CHECK(spec.requestType == RECOMMENDATION);
}

TEST_CASE("ConcretePlantSpecificationBuilder - Default construction") {
    ConcretePlantSpecificationBuilder builder;
    auto spec = builder.getResult();
    
    // Should have default values
    CHECK(spec.waterReq == WaterRequirement::MEDIUM);
    CHECK(spec.seasonReq == Season::YEAR_ROUND);
    CHECK(spec.explicitName.empty());
    CHECK(spec.decorators.empty());
    CHECK(spec.requestType == RECOMMENDATION);
}

TEST_CASE("Builder - RECOMMENDATION request with water and season") {
    ConcretePlantSpecificationBuilder builder;
    
    builder.setRequestType(RECOMMENDATION);
    builder.setWaterRequirement(WaterRequirement::VERY_LOW);
    builder.setSeasonRequirement(Season::YEAR_ROUND);
    
    auto spec = builder.getResult();
    
    CHECK(spec.requestType == RECOMMENDATION);
    CHECK(spec.waterReq == WaterRequirement::VERY_LOW);
    CHECK(spec.seasonReq == Season::YEAR_ROUND);
    CHECK(spec.explicitName.empty());
    CHECK(spec.decorators.empty());
}

TEST_CASE("Builder - RECOMMENDATION with different water requirements") {
    ConcretePlantSpecificationBuilder builder;
    builder.setRequestType(RECOMMENDATION);
    
    SUBCASE("VERY_LOW water") {
        builder.setWaterRequirement(WaterRequirement::VERY_LOW);
        auto spec = builder.getResult();
        CHECK(spec.waterReq == WaterRequirement::VERY_LOW);
    }
    
    SUBCASE("LOW water") {
        builder.setWaterRequirement(WaterRequirement::LOW);
        auto spec = builder.getResult();
        CHECK(spec.waterReq == WaterRequirement::LOW);
    }
    
    SUBCASE("MEDIUM water") {
        builder.setWaterRequirement(WaterRequirement::MEDIUM);
        auto spec = builder.getResult();
        CHECK(spec.waterReq == WaterRequirement::MEDIUM);
    }
    
    SUBCASE("HIGH water") {
        builder.setWaterRequirement(WaterRequirement::HIGH);
        auto spec = builder.getResult();
        CHECK(spec.waterReq == WaterRequirement::HIGH);
    }
}

TEST_CASE("Builder - RECOMMENDATION with different seasons") {
    ConcretePlantSpecificationBuilder builder;
    builder.setRequestType(RECOMMENDATION);
    
    SUBCASE("SPRING season") {
        builder.setSeasonRequirement(Season::SPRING);
        auto spec = builder.getResult();
        CHECK(spec.seasonReq == Season::SPRING);
    }
    
    SUBCASE("SUMMER season") {
        builder.setSeasonRequirement(Season::SUMMER);
        auto spec = builder.getResult();
        CHECK(spec.seasonReq == Season::SUMMER);
    }
    
    SUBCASE("FALL season") {
        builder.setSeasonRequirement(Season::FALL);
        auto spec = builder.getResult();
        CHECK(spec.seasonReq == Season::FALL);
    }
    
    SUBCASE("WINTER season") {
        builder.setSeasonRequirement(Season::WINTER);
        auto spec = builder.getResult();
        CHECK(spec.seasonReq == Season::WINTER);
    }
    
    SUBCASE("YEAR_ROUND season") {
        builder.setSeasonRequirement(Season::YEAR_ROUND);
        auto spec = builder.getResult();
        CHECK(spec.seasonReq == Season::YEAR_ROUND);
    }
}

TEST_CASE("Builder - PURCHASE request with plant name") {
    ConcretePlantSpecificationBuilder builder;
    
    builder.setRequestType(PURCHASE);
    builder.setExplicitName("Rose");
    
    auto spec = builder.getResult();
    
    CHECK(spec.requestType == PURCHASE);
    CHECK(spec.explicitName == "Rose");
    CHECK(spec.decorators.empty());
}

TEST_CASE("Builder - PURCHASE request with different plant names") {
    ConcretePlantSpecificationBuilder builder;
    builder.setRequestType(PURCHASE);
    
    SUBCASE("Rose") {
        builder.setExplicitName("Rose");
        CHECK(builder.getResult().explicitName == "Rose");
    }
    
    SUBCASE("Cactus") {
        builder.setExplicitName("Cactus");
        CHECK(builder.getResult().explicitName == "Cactus");
    }
    
    SUBCASE("Basil") {
        builder.setExplicitName("Basil");
        CHECK(builder.getResult().explicitName == "Basil");
    }
}

TEST_CASE("Builder - PURCHASE request with single decorator") {
    ConcretePlantSpecificationBuilder builder;
    
    builder.setRequestType(PURCHASE);
    builder.setExplicitName("Rose");
    builder.addDecorator("GiftWrap");
    
    auto spec = builder.getResult();
    
    CHECK(spec.requestType == PURCHASE);
    CHECK(spec.explicitName == "Rose");
    REQUIRE(spec.decorators.size() == 1);
    CHECK(spec.decorators[0] == "GiftWrap");
}

TEST_CASE("Builder - PURCHASE request with multiple decorators") {
    ConcretePlantSpecificationBuilder builder;
    
    builder.setRequestType(PURCHASE);
    builder.setExplicitName("Cactus");
    builder.addDecorator("Pot");
    builder.addDecorator("GiftWrap");
    builder.addDecorator("Ribbon");
    
    auto spec = builder.getResult();
    
    CHECK(spec.requestType == PURCHASE);
    CHECK(spec.explicitName == "Cactus");
    REQUIRE(spec.decorators.size() == 3);
    CHECK(spec.decorators[0] == "Pot");
    CHECK(spec.decorators[1] == "GiftWrap");
    CHECK(spec.decorators[2] == "Ribbon");
}

TEST_CASE("Builder - RECOMMENDATION cannot have decorators") {
    ConcretePlantSpecificationBuilder builder;
    
    builder.setRequestType(RECOMMENDATION);
    builder.setWaterRequirement(WaterRequirement::LOW);
    builder.setSeasonRequirement(Season::SPRING);
    builder.addDecorator("GiftWrap");  // Should be ignored
    builder.addDecorator("Pot");       // Should be ignored
    
    auto spec = builder.getResult();
    
    CHECK(spec.requestType == RECOMMENDATION);
    CHECK(spec.decorators.empty());  // Decorators should not be added
}

TEST_CASE("Builder - Decorators added before setting PURCHASE type are ignored") {
    ConcretePlantSpecificationBuilder builder;
    
    // Try to add decorators before setting type (defaults to RECOMMENDATION)
    builder.addDecorator("GiftWrap");
    builder.addDecorator("Pot");
    
    auto spec1 = builder.getResult();
    CHECK(spec1.decorators.empty());  // Should be ignored
    
    // Now set to PURCHASE and add decorators
    builder.setRequestType(PURCHASE);
    builder.addDecorator("Ribbon");
    
    auto spec2 = builder.getResult();
    REQUIRE(spec2.decorators.size() == 1);
    CHECK(spec2.decorators[0] == "Ribbon");  // Only this one should be added
}

TEST_CASE("Builder - reset() clears all fields") {
    ConcretePlantSpecificationBuilder builder;
    
    // Build a complex specification
    builder.setRequestType(PURCHASE);
    builder.setExplicitName("Rose");
    builder.setWaterRequirement(WaterRequirement::HIGH);
    builder.setSeasonRequirement(Season::SUMMER);
    builder.addDecorator("GiftWrap");
    builder.addDecorator("Pot");
    
    // Verify it's populated
    auto spec1 = builder.getResult();
    CHECK(spec1.requestType == PURCHASE);
    CHECK(spec1.explicitName == "Rose");
    CHECK_FALSE(spec1.decorators.empty());
    
    // Reset the builder
    builder.reset();
    
    // Should be back to defaults
    auto spec2 = builder.getResult();
    CHECK(spec2.requestType == RECOMMENDATION);
    CHECK(spec2.waterReq == WaterRequirement::MEDIUM);
    CHECK(spec2.seasonReq == Season::YEAR_ROUND);
    CHECK(spec2.explicitName.empty());
    CHECK(spec2.decorators.empty());
}

TEST_CASE("Builder - Can reuse builder after reset") {
    ConcretePlantSpecificationBuilder builder;
    
    // First specification - RECOMMENDATION
    builder.setRequestType(RECOMMENDATION);
    builder.setWaterRequirement(WaterRequirement::VERY_LOW);
    builder.setSeasonRequirement(Season::YEAR_ROUND);
    auto spec1 = builder.getResult();
    
    CHECK(spec1.requestType == RECOMMENDATION);
    CHECK(spec1.waterReq == WaterRequirement::VERY_LOW);
    
    // Reset and build new specification - PURCHASE
    builder.reset();
    builder.setRequestType(PURCHASE);
    builder.setExplicitName("Basil");
    builder.addDecorator("GiftWrap");
    auto spec2 = builder.getResult();
    
    CHECK(spec2.requestType == PURCHASE);
    CHECK(spec2.explicitName == "Basil");
    CHECK(spec2.decorators.size() == 1);
    
    // Verify first spec wasn't affected
    CHECK(spec1.requestType == RECOMMENDATION);
    CHECK(spec1.explicitName.empty());
}

TEST_CASE("Builder - Complex RECOMMENDATION scenario") {
    ConcretePlantSpecificationBuilder builder;
    
    // Simulate customer: "I want a high-water plant for summer"
    builder.setRequestType(RECOMMENDATION);
    builder.setWaterRequirement(WaterRequirement::HIGH);
    builder.setSeasonRequirement(Season::SUMMER);
    
    auto spec = builder.getResult();
    
    // This should match plants like Mint (high water, summer)
    CHECK(spec.requestType == RECOMMENDATION);
    CHECK(spec.waterReq == WaterRequirement::HIGH);
    CHECK(spec.seasonReq == Season::SUMMER);
    CHECK(spec.explicitName.empty());
    CHECK(spec.decorators.empty());
}

TEST_CASE("Builder - Complex PURCHASE scenario") {
    ConcretePlantSpecificationBuilder builder;
    
    // Simulate customer: "I want a Rose with gift wrap and a pot"
    builder.setRequestType(PURCHASE);
    builder.setExplicitName("Rose");
    builder.addDecorator("GiftWrap");
    builder.addDecorator("Pot");
    
    auto spec = builder.getResult();
    
    CHECK(spec.requestType == PURCHASE);
    CHECK(spec.explicitName == "Rose");
    REQUIRE(spec.decorators.size() == 2);
    CHECK(spec.decorators[0] == "GiftWrap");
    CHECK(spec.decorators[1] == "Pot");
}

TEST_CASE("Builder - Order of method calls doesn't matter for PURCHASE") {
    ConcretePlantSpecificationBuilder builder;
    
    // Add decorators before setting name and type
    builder.addDecorator("GiftWrap");  // Will be ignored (not PURCHASE yet)
    builder.setExplicitName("Cactus");
    builder.setRequestType(PURCHASE);
    builder.addDecorator("Pot");       // This should be added
    
    auto spec = builder.getResult();
    
    CHECK(spec.requestType == PURCHASE);
    CHECK(spec.explicitName == "Cactus");
    REQUIRE(spec.decorators.size() == 1);
    CHECK(spec.decorators[0] == "Pot");
}

TEST_CASE("Builder - Empty plant name is allowed") {
    ConcretePlantSpecificationBuilder builder;
    
    builder.setRequestType(PURCHASE);
    builder.setExplicitName("");  // Empty name
    
    auto spec = builder.getResult();
    
    CHECK(spec.requestType == PURCHASE);
    CHECK(spec.explicitName.empty());
}

TEST_CASE("Builder - Multiple getResult() calls return same values") {
    ConcretePlantSpecificationBuilder builder;
    
    builder.setRequestType(PURCHASE);
    builder.setExplicitName("Orchid");
    builder.addDecorator("Ribbon");
    
    auto spec1 = builder.getResult();
    auto spec2 = builder.getResult();
    auto spec3 = builder.getResult();
    
    // All should be identical
    CHECK(spec1.requestType == spec2.requestType);
    CHECK(spec1.requestType == spec3.requestType);
    CHECK(spec1.explicitName == spec2.explicitName);
    CHECK(spec1.explicitName == spec3.explicitName);
    CHECK(spec1.decorators.size() == spec2.decorators.size());
    CHECK(spec1.decorators.size() == spec3.decorators.size());
}
