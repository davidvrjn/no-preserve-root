#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../include/doctest.h"

#include "../include/Components/Aloe.h"
#include "../include/Components/Bamboo.h"
#include "../include/Components/Basil.h"
#include "../include/Components/Cactus.h"
#include "../include/Components/Daisy.h"
#include "../include/Components/Fern.h"
#include "../include/Components/Ivy.h"
#include "../include/Components/Lavender.h"
#include "../include/Components/Marigold.h"
#include "../include/Components/Mint.h"
#include "../include/Components/Orchid.h"
#include "../include/Components/Petunia.h"
#include "../include/Components/Rose.h"
#include "../include/Components/SnakePlant.h"
#include "../include/Components/Succulent.h"
#include "../include/Components/Sunflower.h"
#include "../include/Components/Tulip.h"
#include "../include/Patterns/State/PlantState.h"

TEST_CASE("Plant pricing - Budget tier (R85-R95)") {
    SUBCASE("Marigold - R85") {
        auto plant = std::make_shared<Marigold>();
        CHECK_EQ(plant->getPrice(), 85.0);
        CHECK_EQ(plant->getName(), "Marigold");
    }
    
    SUBCASE("Basil - R90") {
        auto plant = std::make_shared<Basil>();
        CHECK_EQ(plant->getPrice(), 90.0);
        CHECK_EQ(plant->getName(), "Basil");
    }
    
    SUBCASE("Succulent - R95") {
        auto plant = std::make_shared<Succulent>();
        CHECK_EQ(plant->getPrice(), 95.0);
        CHECK_EQ(plant->getName(), "Succulent");
    }
}

TEST_CASE("Plant pricing - Standard tier (R100-R105)") {
    SUBCASE("Daisy - R100") {
        auto plant = std::make_shared<Daisy>();
        CHECK_EQ(plant->getPrice(), 100.0);
    }
    
    SUBCASE("Lavender - R100") {
        auto plant = std::make_shared<Lavender>();
        CHECK_EQ(plant->getPrice(), 100.0);
    }
    
    SUBCASE("Mint - R105") {
        auto plant = std::make_shared<Mint>();
        CHECK_EQ(plant->getPrice(), 105.0);
    }
    
    SUBCASE("Tulip - R105") {
        auto plant = std::make_shared<Tulip>();
        CHECK_EQ(plant->getPrice(), 105.0);
    }
}

TEST_CASE("Plant pricing - Mid-tier (R110-R120)") {
    SUBCASE("Bamboo - R110") {
        auto plant = std::make_shared<Bamboo>();
        CHECK_EQ(plant->getPrice(), 110.0);
    }
    
    SUBCASE("Ivy - R110") {
        auto plant = std::make_shared<Ivy>();
        CHECK_EQ(plant->getPrice(), 110.0);
    }
    
    SUBCASE("Petunia - R110") {
        auto plant = std::make_shared<Petunia>();
        CHECK_EQ(plant->getPrice(), 110.0);
    }
    
    SUBCASE("Aloe - R110") {
        auto plant = std::make_shared<Aloe>();
        CHECK_EQ(plant->getPrice(), 110.0);
    }
    
    SUBCASE("Sunflower - R120") {
        auto plant = std::make_shared<Sunflower>();
        CHECK_EQ(plant->getPrice(), 120.0);
    }
    
    SUBCASE("Cactus - R120") {
        auto plant = std::make_shared<Cactus>();
        CHECK_EQ(plant->getPrice(), 120.0);
    }
    
    SUBCASE("SnakePlant - R120") {
        auto plant = std::make_shared<SnakePlant>();
        CHECK_EQ(plant->getPrice(), 120.0);
    }
}

TEST_CASE("Plant pricing - Premium tier (R130-R145)") {
    SUBCASE("Fern - R130") {
        auto plant = std::make_shared<Fern>();
        CHECK_EQ(plant->getPrice(), 130.0);
    }
    
    SUBCASE("Rose - R135") {
        auto plant = std::make_shared<Rose>();
        CHECK_EQ(plant->getPrice(), 135.0);
    }
    
    SUBCASE("Orchid - R145") {
        auto plant = std::make_shared<Orchid>();
        CHECK_EQ(plant->getPrice(), 145.0);
    }
}

TEST_CASE("Water requirements - VERY_LOW") {
    SUBCASE("Cactus - 1/day") {
        auto plant = std::make_shared<Cactus>();
        CHECK_EQ(plant->getWaterRequirement(), WaterRequirement::VERY_LOW);
        CHECK_EQ(plant->getWaterConsumption(), 1);
    }
    
    SUBCASE("SnakePlant - 1/day") {
        auto plant = std::make_shared<SnakePlant>();
        CHECK_EQ(plant->getWaterRequirement(), WaterRequirement::VERY_LOW);
        CHECK_EQ(plant->getWaterConsumption(), 1);
    }
    
    SUBCASE("Succulent - 2/day") {
        auto plant = std::make_shared<Succulent>();
        CHECK_EQ(plant->getWaterRequirement(), WaterRequirement::VERY_LOW);
        CHECK_EQ(plant->getWaterConsumption(), 2);
    }
    
    SUBCASE("Aloe - 2/day") {
        auto plant = std::make_shared<Aloe>();
        CHECK_EQ(plant->getWaterRequirement(), WaterRequirement::VERY_LOW);
        CHECK_EQ(plant->getWaterConsumption(), 2);
    }
}

TEST_CASE("Water requirements - LOW") {
    SUBCASE("Ivy - 4/day") {
        auto plant = std::make_shared<Ivy>();
        CHECK_EQ(plant->getWaterRequirement(), WaterRequirement::LOW);
        CHECK_EQ(plant->getWaterConsumption(), 4);
    }
    
    SUBCASE("Lavender - 3/day") {
        auto plant = std::make_shared<Lavender>();
        CHECK_EQ(plant->getWaterRequirement(), WaterRequirement::LOW);
        CHECK_EQ(plant->getWaterConsumption(), 3);
    }
    
    SUBCASE("Marigold - 4/day") {
        auto plant = std::make_shared<Marigold>();
        CHECK_EQ(plant->getWaterRequirement(), WaterRequirement::LOW);
        CHECK_EQ(plant->getWaterConsumption(), 4);
    }
    
    SUBCASE("Orchid - 4/day") {
        auto plant = std::make_shared<Orchid>();
        CHECK_EQ(plant->getWaterRequirement(), WaterRequirement::LOW);
        CHECK_EQ(plant->getWaterConsumption(), 4);
    }
}

TEST_CASE("Water requirements - MEDIUM") {
    SUBCASE("Daisy - 5/day") {
        auto plant = std::make_shared<Daisy>();
        CHECK_EQ(plant->getWaterRequirement(), WaterRequirement::MEDIUM);
        CHECK_EQ(plant->getWaterConsumption(), 5);
    }
    
    SUBCASE("Petunia - 5/day") {
        auto plant = std::make_shared<Petunia>();
        CHECK_EQ(plant->getWaterRequirement(), WaterRequirement::MEDIUM);
        CHECK_EQ(plant->getWaterConsumption(), 5);
    }
    
    SUBCASE("Tulip - 5/day") {
        auto plant = std::make_shared<Tulip>();
        CHECK_EQ(plant->getWaterRequirement(), WaterRequirement::MEDIUM);
        CHECK_EQ(plant->getWaterConsumption(), 5);
    }
    
    SUBCASE("Bamboo - 6/day") {
        auto plant = std::make_shared<Bamboo>();
        CHECK_EQ(plant->getWaterRequirement(), WaterRequirement::MEDIUM);
        CHECK_EQ(plant->getWaterConsumption(), 6);
    }
    
    SUBCASE("Basil - 6/day") {
        auto plant = std::make_shared<Basil>();
        CHECK_EQ(plant->getWaterRequirement(), WaterRequirement::MEDIUM);
        CHECK_EQ(plant->getWaterConsumption(), 6);
    }
    
    SUBCASE("Rose - 6/day") {
        auto plant = std::make_shared<Rose>();
        CHECK_EQ(plant->getWaterRequirement(), WaterRequirement::MEDIUM);
        CHECK_EQ(plant->getWaterConsumption(), 6);
    }
}

TEST_CASE("Water requirements - HIGH") {
    SUBCASE("Mint - 7/day") {
        auto plant = std::make_shared<Mint>();
        CHECK_EQ(plant->getWaterRequirement(), WaterRequirement::HIGH);
        CHECK_EQ(plant->getWaterConsumption(), 7);
    }
    
    SUBCASE("Sunflower - 7/day") {
        auto plant = std::make_shared<Sunflower>();
        CHECK_EQ(plant->getWaterRequirement(), WaterRequirement::HIGH);
        CHECK_EQ(plant->getWaterConsumption(), 7);
    }
    
    SUBCASE("Fern - 8/day") {
        auto plant = std::make_shared<Fern>();
        CHECK_EQ(plant->getWaterRequirement(), WaterRequirement::HIGH);
        CHECK_EQ(plant->getWaterConsumption(), 8);
    }
}

TEST_CASE("Growth times - Fast plants (3 days total)") {
    SUBCASE("Bamboo - 1d seedling + 2d growing") {
        auto plant = std::make_shared<Bamboo>();
        CHECK_EQ(plant->getSeedlingDuration(), 1);
        CHECK_EQ(plant->getGrowingDuration(), 2);
    }
    
    SUBCASE("Basil - 1d seedling + 2d growing") {
        auto plant = std::make_shared<Basil>();
        CHECK_EQ(plant->getSeedlingDuration(), 1);
        CHECK_EQ(plant->getGrowingDuration(), 2);
    }
    
    SUBCASE("Marigold - 1d seedling + 2d growing") {
        auto plant = std::make_shared<Marigold>();
        CHECK_EQ(plant->getSeedlingDuration(), 1);
        CHECK_EQ(plant->getGrowingDuration(), 2);
    }
    
    SUBCASE("Mint - 1d seedling + 2d growing") {
        auto plant = std::make_shared<Mint>();
        CHECK_EQ(plant->getSeedlingDuration(), 1);
        CHECK_EQ(plant->getGrowingDuration(), 2);
    }
}

TEST_CASE("Growth times - Medium speed plants (4-6 days total)") {
    SUBCASE("Daisy - 4 days (1d + 3d)") {
        auto plant = std::make_shared<Daisy>();
        CHECK_EQ(plant->getSeedlingDuration(), 1);
        CHECK_EQ(plant->getGrowingDuration(), 3);
    }
    
    SUBCASE("Lavender - 5 days (2d + 3d)") {
        auto plant = std::make_shared<Lavender>();
        CHECK_EQ(plant->getSeedlingDuration(), 2);
        CHECK_EQ(plant->getGrowingDuration(), 3);
    }
    
    SUBCASE("Rose - 6 days (2d + 4d)") {
        auto plant = std::make_shared<Rose>();
        CHECK_EQ(plant->getSeedlingDuration(), 2);
        CHECK_EQ(plant->getGrowingDuration(), 4);
    }
    
    SUBCASE("Fern - 6 days (2d + 4d)") {
        auto plant = std::make_shared<Fern>();
        CHECK_EQ(plant->getSeedlingDuration(), 2);
        CHECK_EQ(plant->getGrowingDuration(), 4);
    }
}

TEST_CASE("Growth times - Slow plants (7 days total)") {
    SUBCASE("Cactus - 3d seedling + 4d growing") {
        auto plant = std::make_shared<Cactus>();
        CHECK_EQ(plant->getSeedlingDuration(), 3);
        CHECK_EQ(plant->getGrowingDuration(), 4);
    }
    
    SUBCASE("Orchid - 3d seedling + 4d growing") {
        auto plant = std::make_shared<Orchid>();
        CHECK_EQ(plant->getSeedlingDuration(), 3);
        CHECK_EQ(plant->getGrowingDuration(), 4);
    }
    
    SUBCASE("SnakePlant - 3d seedling + 4d growing") {
        auto plant = std::make_shared<SnakePlant>();
        CHECK_EQ(plant->getSeedlingDuration(), 3);
        CHECK_EQ(plant->getGrowingDuration(), 4);
    }
}

TEST_CASE("Growth time validation - Balanced for gameplay (1-3 days seedling, 2-4 days growing)") {
    std::vector<std::shared_ptr<Plant>> allPlants = {
        std::make_shared<Aloe>(),
        std::make_shared<Bamboo>(),
        std::make_shared<Basil>(),
        std::make_shared<Cactus>(),
        std::make_shared<Daisy>(),
        std::make_shared<Fern>(),
        std::make_shared<Ivy>(),
        std::make_shared<Lavender>(),
        std::make_shared<Marigold>(),
        std::make_shared<Mint>(),
        std::make_shared<Orchid>(),
        std::make_shared<Petunia>(),
        std::make_shared<Rose>(),
        std::make_shared<SnakePlant>(),
        std::make_shared<Succulent>(),
        std::make_shared<Sunflower>(),
        std::make_shared<Tulip>()
    };
    
    for (const auto& plant : allPlants) {
        int seedling = plant->getSeedlingDuration();
        int growing = plant->getGrowingDuration();
        
        CHECK_GE(seedling, 1);  // Minimum 1 day seedling
        CHECK_LE(seedling, 3);  // Maximum 3 days seedling
        CHECK_GE(growing, 2);   // Minimum 2 days growing
        CHECK_LE(growing, 4);   // Maximum 4 days growing
        
        int total = seedling + growing;
        CHECK_GE(total, 3);     // Minimum 3 days total
        CHECK_LE(total, 7);     // Maximum 7 days total
    }
}

TEST_CASE("Seasonal availability - Year-round plants") {
    SUBCASE("Cactus - year-round") {
        auto plant = std::make_shared<Cactus>();
        auto seasons = plant->getPreferredSeasons();
        CHECK_EQ(seasons.size(), 1);
        CHECK_EQ(seasons[0], Season::YEAR_ROUND);
    }
    
    SUBCASE("Bamboo - year-round") {
        auto plant = std::make_shared<Bamboo>();
        auto seasons = plant->getPreferredSeasons();
        CHECK_EQ(seasons.size(), 1);
        CHECK_EQ(seasons[0], Season::YEAR_ROUND);
    }
    
    SUBCASE("Aloe - year-round") {
        auto plant = std::make_shared<Aloe>();
        auto seasons = plant->getPreferredSeasons();
        CHECK_EQ(seasons.size(), 1);
        CHECK_EQ(seasons[0], Season::YEAR_ROUND);
    }
}

TEST_CASE("Seasonal availability - Single season plants") {
    SUBCASE("Basil - summer only") {
        auto plant = std::make_shared<Basil>();
        auto seasons = plant->getPreferredSeasons();
        CHECK_EQ(seasons.size(), 1);
        CHECK_EQ(seasons[0], Season::SUMMER);
    }
    
    SUBCASE("Tulip - spring only") {
        auto plant = std::make_shared<Tulip>();
        auto seasons = plant->getPreferredSeasons();
        CHECK_EQ(seasons.size(), 1);
        CHECK_EQ(seasons[0], Season::SPRING);
    }
    
    SUBCASE("Sunflower - summer only") {
        auto plant = std::make_shared<Sunflower>();
        auto seasons = plant->getPreferredSeasons();
        CHECK_EQ(seasons.size(), 1);
        CHECK_EQ(seasons[0], Season::SUMMER);
    }
}

TEST_CASE("Seasonal availability - Multi-season plants") {
    SUBCASE("Lavender - spring & summer") {
        auto plant = std::make_shared<Lavender>();
        auto seasons = plant->getPreferredSeasons();
        CHECK_EQ(seasons.size(), 2);
    }
    
    SUBCASE("Rose - spring, summer, fall") {
        auto plant = std::make_shared<Rose>();
        auto seasons = plant->getPreferredSeasons();
        CHECK_EQ(seasons.size(), 3);
    }
    
    SUBCASE("Mint - spring, summer, fall") {
        auto plant = std::make_shared<Mint>();
        auto seasons = plant->getPreferredSeasons();
        CHECK_EQ(seasons.size(), 3);
    }
}

TEST_CASE("Water mechanics - Watering increases water level") {
    auto plant = std::make_shared<Rose>();
    plant->setWaterLevel(50);
    
    plant->water();
    
    CHECK_GT(plant->getWaterLevel(), 50);  // Should increase
    CHECK_LE(plant->getWaterLevel(), 100); // Should not exceed 100
}

TEST_CASE("Water mechanics - Different plants have different refill amounts") {
    SUBCASE("High water plant refills more") {
        auto fern = std::make_shared<Fern>();
        fern->setWaterLevel(0);
        fern->water();
        int fernRefill = fern->getWaterLevel();
        
        auto cactus = std::make_shared<Cactus>();
        cactus->setWaterLevel(0);
        cactus->water();
        int cactusRefill = cactus->getWaterLevel();
        
        CHECK_GT(fernRefill, cactusRefill);  // High water plant refills more
    }
}

TEST_CASE("Water mechanics - Water level caps at 100") {
    auto plant = std::make_shared<Rose>();
    plant->setWaterLevel(95);
    
    plant->water();
    
    CHECK_EQ(plant->getWaterLevel(), 100);  // Should cap at 100
}


TEST_CASE("Prototype - clone() preserves state and ID") {
    auto original = std::make_shared<Rose>();
    original->setId(12345);
    original->setAge(50);
    original->setHealth(75);
    original->setWaterLevel(60);
    
    auto cloned = std::dynamic_pointer_cast<Plant>(original->clone());
    
    CHECK_EQ(cloned->getId(), 12345);           // Same ID
    CHECK_EQ(cloned->getAge(), 50);             // Same age
    CHECK_EQ(cloned->getHealth(), 75);          // Same health
    CHECK_EQ(cloned->getWaterLevel(), 60);      // Same water level
}

TEST_CASE("Prototype - blueprintClone() creates fresh seedling") {
    auto original = std::make_shared<Rose>();
    original->setId(12345);
    original->setAge(50);
    original->setHealth(75);
    original->setWaterLevel(60);
    
    auto fresh = std::dynamic_pointer_cast<Plant>(original->blueprintClone());
    
    CHECK_NE(fresh->getId(), 12345);            // Different ID
    CHECK_EQ(fresh->getAge(), 0);               // Fresh seedling
    CHECK_EQ(fresh->getHealth(), 100);          // Full health
    CHECK_EQ(fresh->getWaterLevel(), 100);      // Full water
    CHECK_EQ(fresh->getPrice(), original->getPrice());  // Same price/type
}

TEST_CASE("Prototype - All plants support both clone types") {
    std::vector<std::shared_ptr<Plant>> allPlants = {
        std::make_shared<Aloe>(),
        std::make_shared<Bamboo>(),
        std::make_shared<Basil>(),
        std::make_shared<Cactus>(),
        std::make_shared<Daisy>(),
        std::make_shared<Fern>(),
        std::make_shared<Ivy>(),
        std::make_shared<Lavender>(),
        std::make_shared<Marigold>(),
        std::make_shared<Mint>(),
        std::make_shared<Orchid>(),
        std::make_shared<Petunia>(),
        std::make_shared<Rose>(),
        std::make_shared<SnakePlant>(),
        std::make_shared<Succulent>(),
        std::make_shared<Sunflower>(),
        std::make_shared<Tulip>()
    };
    
    for (const auto& plant : allPlants) {
        auto cloned = plant->clone();
        auto blueprint = plant->blueprintClone();
        
        CHECK(cloned.get() != nullptr);
        CHECK(blueprint.get() != nullptr);
    }
}

TEST_CASE("Type names - All plants return correct type") {
    CHECK_EQ(std::make_shared<Aloe>()->typeName(), "Aloe");
    CHECK_EQ(std::make_shared<Bamboo>()->typeName(), "Bamboo");
    CHECK_EQ(std::make_shared<Basil>()->typeName(), "Basil");
    CHECK_EQ(std::make_shared<Cactus>()->typeName(), "Cactus");
    CHECK_EQ(std::make_shared<Daisy>()->typeName(), "Daisy");
    CHECK_EQ(std::make_shared<Fern>()->typeName(), "Fern");
    CHECK_EQ(std::make_shared<Ivy>()->typeName(), "Ivy");
    CHECK_EQ(std::make_shared<Lavender>()->typeName(), "Lavender");
    CHECK_EQ(std::make_shared<Marigold>()->typeName(), "Marigold");
    CHECK_EQ(std::make_shared<Mint>()->typeName(), "Mint");
    CHECK_EQ(std::make_shared<Orchid>()->typeName(), "Orchid");
    CHECK_EQ(std::make_shared<Petunia>()->typeName(), "Petunia");
    CHECK_EQ(std::make_shared<Rose>()->typeName(), "Rose");
    CHECK_EQ(std::make_shared<SnakePlant>()->typeName(), "SnakePlant");
    CHECK_EQ(std::make_shared<Succulent>()->typeName(), "Succulent");
    CHECK_EQ(std::make_shared<Sunflower>()->typeName(), "Sunflower");
    CHECK_EQ(std::make_shared<Tulip>()->typeName(), "Tulip");
}

TEST_CASE("Initial state - All plants start as fresh seedlings") {
    std::vector<std::shared_ptr<Plant>> allPlants = {
        std::make_shared<Aloe>(),
        std::make_shared<Bamboo>(),
        std::make_shared<Basil>(),
        std::make_shared<Cactus>(),
        std::make_shared<Daisy>(),
        std::make_shared<Fern>(),
        std::make_shared<Ivy>(),
        std::make_shared<Lavender>(),
        std::make_shared<Marigold>(),
        std::make_shared<Mint>(),
        std::make_shared<Orchid>(),
        std::make_shared<Petunia>(),
        std::make_shared<Rose>(),
        std::make_shared<SnakePlant>(),
        std::make_shared<Succulent>(),
        std::make_shared<Sunflower>(),
        std::make_shared<Tulip>()
    };
    
    for (const auto& plant : allPlants) {
        CHECK_EQ(plant->getAge(), 0);           // Fresh plant
        CHECK_EQ(plant->getHealth(), 100);      // Full health
        CHECK_EQ(plant->getWaterLevel(), 100);  // Full water
    }
}
