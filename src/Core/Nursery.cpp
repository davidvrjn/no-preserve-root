#include "../../include/Core/Nursery.h"

#include <algorithm>
#include <map>
#include <random>
#include <sstream>
#include <stdexcept>

#include "../../include/Actors/Customer.h"
#include "../../include/Actors/Staff.h"
#include "../../include/Components/Plant.h"
#include "../../include/Components/PlantAttributes.h"
#include "../../include/Core/Inventory.h"
#include "../../include/Patterns/Builder/ConcretePlantSpecificationBuilder.h"
#include "../../include/Patterns/Command/Command.h"
#include "../../include/Patterns/Command/FulfillCustomerCommand.h"
#include "../../include/Patterns/Memento/Memento.h"
#include "../../include/json.hpp"

Nursery::Nursery()
    : currentDay(0),
      currentStep(0),
      currentPhase(GamePhase::IDLE),
      money(1000.0),
      reputation(50),                           // Start at 50/100 (neutral)
      inventory(std::make_shared<Inventory>())  // Initialize inventory
{}

Nursery::~Nursery() = default;

void Nursery::runSimulation() {
    // "Speed through day" mode: auto-execute all steps, pause at DAY_END
    startNewDay();
    
    while (!isDayComplete()) {
        advanceStep();
    }
    
    // Now in DAY_END phase - user can save/hire
    // User must call finishDay() before next day
}

void Nursery::startNewDay() {
    // If coming from DAY_END, allow transition to next day
    // If IDLE, also allow (first day)
    // Otherwise, error
    if (currentPhase != GamePhase::IDLE && currentPhase != GamePhase::DAY_END) {
        throw std::runtime_error("Cannot start new day: day in progress. Complete current day first.");
    }

    // Clear any remaining commands from previous day (staff clocked out)
    // Plant care commands that weren't completed yesterday are discarded
    while (!requestQueue.empty()) {
        requestQueue.pop();
    }

    // Advance to new day
    currentDay++;
    currentStep = 0;
    currentPhase = GamePhase::DAY_START;

    // PLANT UPDATES: Plants grew overnight
    // This triggers state changes and observer notifications (adds WaterPlant commands)
    if (inventory) {
        auto allPlants = inventory->getAllPlants();
        for (auto& plant : allPlants) {
            if (plant) {
                plant->performDailyActivity();  // Delegates to current state
            }
        }
    }

    // Reset all staff to not busy at the start of the day
    auto currentStaff = staffChainHead;
    while (currentStaff) {
        currentStaff->setBusy(false);
        currentStaff = currentStaff->getSuccessor();
    }

    // Now in DAY_START phase - user can plant seeds, view inventory, etc.
    // User should call advanceStep() when ready to begin
}

bool Nursery::advanceStep() {
    if (currentPhase == GamePhase::IDLE) {
        throw std::runtime_error("Cannot advance step: no day in progress. Call startNewDay() first.");
    }

    if (isDayComplete()) {
        return false;  // Already at day end
    }

    // Execute one step (currentStep is 0-4)
    
    // At the start of each step, reset all staff to not busy
    auto currentStaff = staffChainHead;
    while (currentStaff) {
        currentStaff->setBusy(false);
        currentStaff = currentStaff->getSuccessor();
    }

    // Spawn customers based on reputation (random)
    // Higher reputation = more customers and higher spawn chance
    // Can spawn MORE customers than staff to create overflow pressure
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> spawnChanceDist(1, 100);
    double repFactor;

    if (reputation == 0) {
        repFactor = 0;
    }
    else if (reputation <= 25) {
        repFactor = reputation / 2;  // 31-43
    }
    else if (reputation <= 50) {
        repFactor = reputation / 3 + 4; // 43-51
    }
    else if (reputation <= 75) {
        repFactor = reputation / 2 + (reputation/3 -22); // 51-71
    }
    else {
        repFactor = reputation / 1.86; // 71-84
    }
    repFactor = std::ceil(repFactor);

    int spawnChance = 30 + repFactor;

    if (spawnChanceDist(gen) <= spawnChance) {
        // Determine number of customers: scales with reputation
        // 0-25 rep: 1 customer
        // 26-50 rep: 1-3 customers (avg 2)
        // 51-75 rep: 2-5 customers (avg 3.5)
        // 76-100 rep: 4-9 customers (avg 6.5)
        
        int minCustomers, maxCustomers;
        
        if (reputation <= 25) {
            minCustomers = 1;
            maxCustomers = 1;
        } else if (reputation <= 50) {
            minCustomers = 1;
            maxCustomers = 3;
        } else if (reputation <= 75) {
            minCustomers = 2;
            maxCustomers = 5;
        } else {
            minCustomers = 4;
            maxCustomers = 9;
        }
        
        std::uniform_int_distribution<> customerCountDist(minCustomers, maxCustomers);
        int numCustomers = customerCountDist(gen);
        
        for (int i = 0; i < numCustomers; ++i) {
            spawnCustomer();  // Adds FulfillCustomerCommand to queue
        }
    }

    // Process commands: continue until queue is empty or all staff are busy
    // The Chain of Responsibility routes each command to the appropriate handler
    while (!requestQueue.empty() && staffChainHead) {
        // Check if all staff are busy - if so, stop processing this step
        bool allBusy = true;
        currentStaff = staffChainHead;
        while (currentStaff) {
            if (!currentStaff->isBusy()) {
                allBusy = false;
                break;
            }
            currentStaff = currentStaff->getSuccessor();
        }
        
        if (allBusy) {
            break;  // All staff busy, wait for next step
        }
        
        // Process one command
        auto cmd = std::move(requestQueue.front());
        requestQueue.pop();
        staffChainHead->handleRequest(std::move(cmd));
    }

    // Customer timeout check: Remove any FulfillCustomerCommands still Pending
    // These are customers who left because no staff was available to serve them
    // Plant care commands (WaterPlantCommand) persist in queue for later steps
    std::queue<std::unique_ptr<Command>> keptCommands;
    int customersWhoLeft = 0;
    
    while (!requestQueue.empty()) {
        auto cmd = std::move(requestQueue.front());
        requestQueue.pop();
        
        auto* customerCmd = dynamic_cast<FulfillCustomerCommand*>(cmd.get());
        if (customerCmd && customerCmd->getStatus() == Command::Status::Pending) {
            // Customer left unserved - don't keep this command
            customersWhoLeft++;
        } else {
            // Keep this command (either not a customer command, or was handled, or is plant care)
            keptCommands.push(std::move(cmd));
        }
    }
    
    requestQueue = std::move(keptCommands);
    
    if (customersWhoLeft > 0) {
        adjustReputation(-3 * customersWhoLeft);
    }

    // Advance step counter
    currentStep++;

    // Update phase
    if (currentStep == 5) {
        currentPhase = GamePhase::DAY_END;  // All steps complete - can save/hire
    } else {
        currentPhase = GamePhase::STEP_BREAK;  // Between steps - can plant/view
    }

    return true;
}

void Nursery::addRequest(std::unique_ptr<Command> cmd) {
    if (cmd) {
        requestQueue.push(std::move(cmd));
    }
}

Memento* Nursery::createMemento() const {
    Memento::NurseryState state;
    state.day = currentDay;

    // Build comprehensive JSON serialization
    std::ostringstream json;
    json << "{";

    // Business metrics
    json << "\"currentDay\":" << currentDay << ",";
    json << "\"money\":" << money << ",";
    json << "\"reputation\":" << reputation << ",";

    // Known plant types array
    json << "\"knownPlantTypes\":[";
    for (size_t i = 0; i < knownPlantTypes.size(); ++i) {
        if (i > 0) json << ",";
        json << "\"" << knownPlantTypes[i] << "\"";
    }
    json << "],";

    // Serialize entire inventory (this will recursively serialize all components)
    json << "\"inventory\":";
    if (inventory) {
        json << inventory->serialize();
    } else {
        json << "null";
    }

    json << "}";

    state.serializedData = json.str();
    return new Memento(state);
}

void Nursery::restoreFromMemento(Memento* memento) {
    if (!memento) return;

    // Parse the serialized JSON data
    auto json = nlohmann::json::parse(memento->getState().serializedData);

    // Restore business metrics
    currentDay = json["currentDay"].get<int>();
    money = json["money"].get<double>();
    reputation = json["reputation"].get<int>();

    // Restore known plant types
    knownPlantTypes.clear();
    for (const auto& type : json["knownPlantTypes"]) {
        knownPlantTypes.push_back(type.get<std::string>());
    }

    // Restore inventory
    if (!json["inventory"].is_null()) {
        inventory = std::make_shared<Inventory>();
        inventory->deserialize(json["inventory"].dump());
    } else {
        inventory = nullptr;
    }
}

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

void Nursery::setupNursery() {}

void Nursery::adjustMoney(double amount) { money += amount; }

void Nursery::adjustReputation(int change) {
    reputation += change;
    // Clamp between 0 and 100
    if (reputation < 0) reputation = 0;
    if (reputation > 100) reputation = 100;
}
