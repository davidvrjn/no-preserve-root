#include "../include/Components/Cactus.h"
#include "../include/Components/Lavender.h"
#include "../include/Components/PlantAttributes.h"
#include "../include/Components/Rose.h"
#include "../include/Components/Tulip.h"
#include "../include/Patterns/State/Seedling.h"
#include "../include/doctest.h"

/**
 * @file test_seasonal_pricing.cpp
 * @brief Tests for seasonal pricing system
 *
 * Pricing Formula (balanced to avoid multi-season advantage):
 * - In-season: basePrice * (1 + 0.3 / numSeasons)
 * - Off-season: basePrice * 0.95 (gentle penalty)
 * - Year-round: basePrice (no adjustment)
 */

TEST_CASE("Seasonal pricing - Year-round plants have no adjustment") {
    auto cactus = std::make_shared<Cactus>();
    cactus->setState(std::make_unique<Seedling>());

    double basePrice = cactus->getPrice();

    // Year-round plants should have same price in all seasons
    CHECK(cactus->getSeasonalPrice(Season::SPRING) == doctest::Approx(basePrice));
    CHECK(cactus->getSeasonalPrice(Season::SUMMER) == doctest::Approx(basePrice));
    CHECK(cactus->getSeasonalPrice(Season::FALL) == doctest::Approx(basePrice));
    CHECK(cactus->getSeasonalPrice(Season::WINTER) == doctest::Approx(basePrice));
}

TEST_CASE("Seasonal pricing - Single season plant (highest in-season bonus)") {
    // Tulip is Spring only
    auto tulip = std::make_shared<Tulip>();
    tulip->setState(std::make_unique<Seedling>());

    double basePrice = tulip->getPrice();

    // In-season: +30% (0.3 / 1 season)
    CHECK(tulip->getSeasonalPrice(Season::SPRING) == doctest::Approx(basePrice * 1.3));

    // Off-season: -5%
    CHECK(tulip->getSeasonalPrice(Season::SUMMER) == doctest::Approx(basePrice * 0.95));
    CHECK(tulip->getSeasonalPrice(Season::FALL) == doctest::Approx(basePrice * 0.95));
    CHECK(tulip->getSeasonalPrice(Season::WINTER) == doctest::Approx(basePrice * 0.95));
}

TEST_CASE("Seasonal pricing - Two season plant (moderate bonus)") {
    // Lavender is Spring + Summer
    auto lavender = std::make_shared<Lavender>();
    lavender->setState(std::make_unique<Seedling>());

    double basePrice = lavender->getPrice();

    // In-season: +15% (0.3 / 2 seasons)
    CHECK(lavender->getSeasonalPrice(Season::SPRING) == doctest::Approx(basePrice * 1.15));
    CHECK(lavender->getSeasonalPrice(Season::SUMMER) == doctest::Approx(basePrice * 1.15));

    // Off-season: -5%
    CHECK(lavender->getSeasonalPrice(Season::FALL) == doctest::Approx(basePrice * 0.95));
    CHECK(lavender->getSeasonalPrice(Season::WINTER) == doctest::Approx(basePrice * 0.95));
}

TEST_CASE("Seasonal pricing - Three season plant (balanced bonus)") {
    // Rose is Spring + Summer + Fall
    auto rose = std::make_shared<Rose>();
    rose->setState(std::make_unique<Seedling>());

    double basePrice = rose->getPrice();

    // In-season: +10% (0.3 / 3 seasons)
    CHECK(rose->getSeasonalPrice(Season::SPRING) == doctest::Approx(basePrice * 1.1).epsilon(0.01));
    CHECK(rose->getSeasonalPrice(Season::SUMMER) == doctest::Approx(basePrice * 1.1).epsilon(0.01));
    CHECK(rose->getSeasonalPrice(Season::FALL) == doctest::Approx(basePrice * 1.1).epsilon(0.01));

    // Off-season: -5%
    CHECK(rose->getSeasonalPrice(Season::WINTER) == doctest::Approx(basePrice * 0.95));
}

TEST_CASE("Seasonal pricing - Balance verification") {
    // Verify that seasonal pricing is balanced across all seasons
    auto tulip = std::make_shared<Tulip>();        // 1 season (Spring)
    auto lavender = std::make_shared<Lavender>();  // 2 seasons (Spring, Summer)
    auto rose = std::make_shared<Rose>();          // 3 seasons (Spring, Summer, Fall)
    auto cactus = std::make_shared<Cactus>();      // year-round

    tulip->setState(std::make_unique<Seedling>());
    lavender->setState(std::make_unique<Seedling>());
    rose->setState(std::make_unique<Seedling>());
    cactus->setState(std::make_unique<Seedling>());

    // Calculate average price across all 4 seasons for each plant type
    auto calcAverage = [](std::shared_ptr<Plant> plant) {
        double total =
            plant->getSeasonalPrice(Season::SPRING) + plant->getSeasonalPrice(Season::SUMMER) +
            plant->getSeasonalPrice(Season::FALL) + plant->getSeasonalPrice(Season::WINTER);
        return total / 4.0;
    };

    double tulipAvg = calcAverage(tulip);
    double lavenderAvg = calcAverage(lavender);
    double roseAvg = calcAverage(rose);
    double cactusAvg = calcAverage(cactus);

    // Year-round average should equal base price
    CHECK(cactusAvg == doctest::Approx(cactus->getPrice()));

    // Verify the average prices are reasonable
    // Formula: (in-season * numSeasons + off-season * (4 - numSeasons)) / 4
    // Tulip: (1.3 * 1 + 0.95 * 3) / 4 = (1.3 + 2.85) / 4 = 1.0375 of base
    // Lavender: (1.15 * 2 + 0.95 * 2) / 4 = (2.3 + 1.9) / 4 = 1.05 of base
    // Rose: (1.1 * 3 + 0.95 * 1) / 4 = (3.3 + 0.95) / 4 = 1.0625 of base

    double tulipBase = tulip->getPrice();
    double lavenderBase = lavender->getPrice();
    double roseBase = rose->getPrice();

    CHECK(tulipAvg == doctest::Approx(tulipBase * 1.0375).epsilon(0.01));
    CHECK(lavenderAvg == doctest::Approx(lavenderBase * 1.05).epsilon(0.01));
    CHECK(roseAvg == doctest::Approx(roseBase * 1.0625).epsilon(0.01));
}
