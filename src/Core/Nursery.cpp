#include "../../include/Core/Nursery.h"

#include <algorithm>
#include <map>
#include <random>
#include <sstream>
#include <stdexcept>

#include "../../include/Actors/Cashier.h"
#include "../../include/Actors/Customer.h"
#include "../../include/Actors/Gardener.h"
#include "../../include/Actors/Staff.h"
#include "../../include/Components/Plant.h"
#include "../../include/Components/PlantAttributes.h"
#include "../../include/Core/Inventory.h"
#include "../../include/Patterns/Builder/ConcretePlantSpecificationBuilder.h"
#include "../../include/Patterns/Command/Command.h"
#include "../../include/Patterns/Command/FulfillCustomerCommand.h"
#include "../../include/Patterns/Memento/Memento.h"
#include "../../include/Utils/CommandLog.h"
#include "../../include/Patterns/Command/LoggingCommand.h"
#include "../../include/json.hpp"

Nursery::Nursery()
    : currentDay(0),
      currentStep(0),
      currentPhase(GamePhase::IDLE),
      money(1000.0),
      reputation(50),                           // Start at 50/100 (neutral)
    inventory(std::make_shared<Inventory>()), // Initialize inventory
    commandLog(std::make_shared<CommandLog>())
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

    // Per-step bookkeeping for UI
    customersLeftThisStep = 0;
    completedCommandsThisStep.clear();
    remainingCommandsAtStepEnd.clear();

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
        // Execute via staff chain as before (commands are wrapped when queued)
        if (staffChainHead) {
            staffChainHead->handleRequest(std::move(cmd));
        }
    }

    // Command cleanup after processing:
    // At this point, the queue contains ONLY unprocessed commands (staff were all busy).
    // All processed commands were executed and destroyed by the staff chain.
    // 
    // Cleanup rules:
    // 1. Pending customer commands: Remove (customers left) + apply -3 reputation penalty
    // 2. Pending plant care commands: Keep (will retry in next step)
    
    std::vector<std::unique_ptr<Command>> allCommands;
    int customersWhoLeft = 0;
    
    // Extract all commands from queue
    while (!requestQueue.empty()) {
        allCommands.push_back(std::move(requestQueue.front()));
        requestQueue.pop();
    }
    
    // Filter: keep only plant care commands, remove customer commands
    for (auto& cmd : allCommands) {
        // Unwrap LoggingCommand if present to check the actual command type
        Command* actualCmd = cmd.get();
        auto* loggingCmd = dynamic_cast<LoggingCommand*>(actualCmd);
        if (loggingCmd) {
            actualCmd = loggingCmd->getInnerCommand();
        }
        
        auto* customerCmd = dynamic_cast<FulfillCustomerCommand*>(actualCmd);

        if (customerCmd) {
            // Customer command that wasn't processed = customer left unserved
            customersWhoLeft++;
            // Don't push back to queue (customer is gone)
        } else {
            // Plant care command that wasn't processed = retry next step
            // We'll re-queue it (LoggingCommand wrapper already recorded pending when queued)
            requestQueue.push(std::move(cmd));
        }
    }

    if (customersWhoLeft > 0) {
        customersLeftThisStep = customersWhoLeft;
        adjustReputation(-3 * customersWhoLeft);
    } else {
        customersLeftThisStep = 0;
    }

    // Populate per-step UI lists from the commandLog (completed and remaining pending)
    completedCommandsThisStep = commandLog->completedTextsForStep(currentStep);
    remainingCommandsAtStepEnd = commandLog->remainingPendingTextsForStep(currentStep);

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
    if (!cmd) return;
    // Wrap the incoming command in a LoggingCommand so we capture Pending and Completed events
    if (commandLog) {
        auto wrapped = std::make_unique<LoggingCommand>(std::move(cmd), commandLog, currentStep);
        requestQueue.push(std::move(wrapped));
    } else {
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
    json << ",";

    // Serialize staff chain
    json << "\"staff\":[";
    auto currentStaff = staffChainHead;
    bool firstStaff = true;
    while (currentStaff) {
        if (!firstStaff) json << ",";
        firstStaff = false;

        json << "{";
        
        // Determine staff type using dynamic_cast
        if (dynamic_cast<Cashier*>(currentStaff.get())) {
            json << "\"type\":\"Cashier\"";
        } else if (dynamic_cast<Gardener*>(currentStaff.get())) {
            json << "\"type\":\"Gardener\"";
        } else {
            json << "\"type\":\"Unknown\"";
        }
        
        json << ",\"busy\":" << (currentStaff->isBusy() ? "true" : "false");
        json << "}";

        currentStaff = currentStaff->getSuccessor();
    }
    json << "]";

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

    // Restore staff chain
    staffChainHead = nullptr;  // Clear existing chain
    
    if (json.contains("staff") && json["staff"].is_array()) {
        std::shared_ptr<Staff> previousStaff = nullptr;
        
        for (const auto& staffJson : json["staff"]) {
            std::shared_ptr<Staff> staff = nullptr;
            
            std::string type = staffJson["type"].get<std::string>();
            
            // Instantiate the correct staff type
            if (type == "Cashier") {
                staff = std::make_shared<Cashier>();
            } else if (type == "Gardener") {
                staff = std::make_shared<Gardener>();
            }
            
            if (staff) {
                // Restore busy state
                if (staffJson.contains("busy")) {
                    staff->setBusy(staffJson["busy"].get<bool>());
                }
                
                // Link into chain
                if (!staffChainHead) {
                    staffChainHead = staff;  // First staff member
                } else if (previousStaff) {
                    previousStaff->setSuccessor(staff);  // Link to previous
                }
                
                previousStaff = staff;
            }
        }
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
