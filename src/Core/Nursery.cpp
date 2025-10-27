#include "../../include/Core/Nursery.h"

#include <algorithm>
#include <map>
#include <random>

#include "../../include/Actors/Customer.h"
#include "../../include/Components/PlantAttributes.h"
#include "../../include/Patterns/Builder/ConcretePlantSpecificationBuilder.h"
#include "../../include/Patterns/Command/Command.h"
#include "../../include/Patterns/Command/FulfillCustomerCommand.h"
#include "../../include/Patterns/Memento/Memento.h"

Nursery::Nursery()
    : currentDay(0),
      money(1000.0),
      reputation(50)  // Start at 50/100 (neutral)
{}

Nursery::~Nursery() = default;

void Nursery::runSimulation() {}

void Nursery::addRequest(std::unique_ptr<Command> cmd) {
    if (cmd) {
        requestQueue.push(std::move(cmd));
    }
}

Memento* Nursery::createMemento() const { return nullptr; }

void Nursery::restoreFromMemento(Memento* memento) { (void)memento; }

Season Nursery::getCurrentSeason() const {
    // Each season lasts 30 days, cycling through SPRING -> SUMMER -> FALL -> WINTER
    int dayInCycle = currentDay % 120;  // 120 days for full year (4 seasons * 30 days)

    if (dayInCycle < 30)
        return Season::SPRING;
    else if (dayInCycle < 60)
        return Season::SUMMER;
    else if (dayInCycle < 90)
        return Season::FALL;
    else
        return Season::WINTER;
}

void Nursery::addKnownPlantType(const std::string& plantType) {
    // Only add if not already in the list
    if (std::find(knownPlantTypes.begin(), knownPlantTypes.end(), plantType) ==
        knownPlantTypes.end()) {
        knownPlantTypes.push_back(plantType);
    }
}

void Nursery::spawnCustomer() {
    // Random number generator for customer request generation
    static std::random_device rd;
    static std::mt19937 gen(rd());

    // Create builder for constructing customer request
    ConcretePlantSpecificationBuilder builder;

    // 35% RECOMMENDATION, 65% PURCHASE
    std::uniform_int_distribution<> requestDist(1, 100);
    bool isPurchase = requestDist(gen) > 35;  // 65% chance

    if (isPurchase) {
        // --- PURCHASE REQUEST ---
        builder.setRequestType(PURCHASE);

        // Determine plant pool: current season plants OR previously grown types
        std::vector<std::string> plantPool;

        // Get plants suitable for current season
        Season currentSeason = getCurrentSeason();

        // Define seasonal plant mappings
        std::map<Season, std::vector<std::string>> seasonalPlants = {
            {Season::SPRING, {"Tulip", "Daisy", "Rose"}},
            {Season::SUMMER, {"Basil", "Lavender", "Marigold", "Mint", "Petunia", "Sunflower"}},
            {Season::FALL, {"Ivy", "Fern", "Bamboo", "Orchid"}},
            {Season::WINTER, {"SnakePlant"}},
            {Season::YEAR_ROUND, {"Aloe", "Cactus", "Succulent"}}};

        // Randomly choose plant source: EITHER seasonal plants OR previously grown types
        std::uniform_int_distribution<> sourceDist(0, 1);
        bool useKnownPlants = (sourceDist(gen) == 1) && !knownPlantTypes.empty();

        if (useKnownPlants) {
            // Use previously grown plant types
            plantPool = knownPlantTypes;
        } else {
            // Use current season plants + year-round plants
            plantPool = seasonalPlants[currentSeason];
            auto yearRound = seasonalPlants[Season::YEAR_ROUND];
            plantPool.insert(plantPool.end(), yearRound.begin(), yearRound.end());
        }

        // Select random plant from pool
        std::uniform_int_distribution<> plantDist(0, plantPool.size() - 1);
        std::string chosenPlant = plantPool[plantDist(gen)];
        builder.setExplicitName(chosenPlant);

        // 65% chance of wanting at least 1 decorator
        std::uniform_int_distribution<> wantsDecoratorDist(1, 100);
        bool wantsDecorators = wantsDecoratorDist(gen) <= 65;

        if (wantsDecorators) {
            // Random number of decorators (1-3)
            std::vector<std::string> availableDecorators = {"GiftWrap", "Pot", "Ribbon"};
            std::uniform_int_distribution<> decoratorCountDist(1, 3);
            int numDecorators = decoratorCountDist(gen);

            // Shuffle decorators to randomize order
            std::shuffle(availableDecorators.begin(), availableDecorators.end(), gen);

            for (int i = 0; i < numDecorators && i < static_cast<int>(availableDecorators.size());
                 ++i) {
                builder.addDecorator(availableDecorators[i]);
            }
        }

    } else {
        // --- RECOMMENDATION REQUEST ---
        builder.setRequestType(RECOMMENDATION);

        // Randomly select water requirement
        std::uniform_int_distribution<> waterDist(0, 3);
        WaterRequirement waterReq = static_cast<WaterRequirement>(waterDist(gen));
        builder.setWaterRequirement(waterReq);

        // Randomly select season
        std::uniform_int_distribution<> seasonDist(0, 4);
        Season season = static_cast<Season>(seasonDist(gen));
        builder.setSeasonRequirement(season);
    }

    // Get the completed specification
    PlantSpecification spec = builder.getResult();

    // Create customer and command
    auto customer = std::make_shared<Customer>();

    auto specPtr = std::make_unique<PlantSpecification>(spec);
    auto command = std::make_unique<FulfillCustomerCommand>(std::move(specPtr), inventory, customer,
                                                            shared_from_this());
    addRequest(std::move(command));
}

void Nursery::processRequestQueue() {}

void Nursery::setupNursery() {}

void Nursery::adjustMoney(double amount) { money += amount; }

void Nursery::adjustReputation(int change) {
    reputation += change;
    // Clamp between 0 and 100
    if (reputation < 0) reputation = 0;
    if (reputation > 100) reputation = 100;
}
