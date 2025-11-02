#include "../../include/Core/Nursery.h"

#include <algorithm>
#include <iostream>
#include <memory>
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
#include "../../include/Patterns/Command/WaterPlantCommand.h"
#include "../../include/Patterns/Command/FertilizeCommand.h"
#include "../../include/Patterns/Command/RemoveWitheredPlantCommand.h"
#include "../../include/Patterns/Memento/Memento.h"
#include "../../include/Utils/CommandLog.h"
#include "../../include/Patterns/Command/LoggingCommand.h"
#include "../../include/json.hpp"
#include "../../include/Patterns/Observer/NurserySupervisor.h"
#include "../../include/Patterns/Factory/RoseFactory.h"
#include "../../include/Patterns/Factory/CactusFactory.h"
#include "../../include/Patterns/Factory/OrchidFactory.h"
#include "../../include/Patterns/Factory/SnakePlantFactory.h"
#include "../../include/Patterns/Factory/DaisyFactory.h"
#include "../../include/Patterns/Factory/SunflowerFactory.h"
#include "../../include/Patterns/Factory/LavenderFactory.h"
#include "../../include/Patterns/Factory/IvyFactory.h"
#include "../../include/Patterns/Factory/SucculentFactory.h"
#include "../../include/Patterns/Factory/TulipFactory.h"
#include "../../include/Patterns/Factory/AloeFactory.h"
#include "../../include/Patterns/Factory/BambooFactory.h"
#include "../../include/Patterns/Factory/FernFactory.h"
#include "../../include/Patterns/Factory/MintFactory.h"
#include "../../include/Patterns/Factory/PetuniaFactory.h"
#include "../../include/Patterns/Factory/BasilFactory.h"
#include "../../include/Patterns/Factory/MarigoldFactory.h"
#include "../../include/Components/Group.h"

Nursery::Nursery()
    : currentDay(0),
      currentStep(0),
      currentPhase(GamePhase::IDLE),
      money(1000.0),
      reputation(50),                           // Start at 50/100 (neutral)
    inventory(std::make_shared<Inventory>()), // Initialize inventory
    commandLog(std::make_shared<CommandLog>()) {
    // Ensure a Storage group exists on newly constructed nurseries (empty by default)
    if (inventory) {
        auto existing = inventory->findGroupByName("Storage");
        if (!existing) {
            auto storage = std::make_shared<Group>("Storage", true);
            inventory->add(storage);
        }
    }
}

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

    // Clear command log from previous day
    if (commandLog) {
        commandLog->clear();
    }

    // PLANT UPDATES: Plants grew overnight
    // This triggers state changes and observer notifications (adds WaterPlant commands)
    if (inventory) {
        auto allPlants = inventory->getAllPlants();
        for (auto& plant : allPlants) {
            if (plant) {
                if (plant->getOwner() != inventory->findGroupByName("Storage"))
                {
                    plant->performDailyActivity();  // Delegates to current state
                }
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
    int roll = spawnChanceDist(gen);

    if (roll <= spawnChance) {
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
        
        customersSpawnedThisStep = numCustomers;
        
        for (int i = 0; i < numCustomers; ++i) {
            spawnCustomer();  // Adds FulfillCustomerCommand to queue
        }
    } else {
        customersSpawnedThisStep = 0;
    }

    // Per-step bookkeeping for UI
    customersLeftThisStep = 0;
    completedCommandsThisStep.clear();
    remainingCommandsAtStepEnd.clear();

    // Process commands via Chain of Responsibility
    // Each command goes through the chain until it finds an available handler
    // NOTE: We need to scan the queue to find commands that CAN be processed,
    // not just process in strict FIFO order, because water commands might block
    // customer commands even when a cashier is available.
    int iterationCount = 0;
    while (!requestQueue.empty() && staffChainHead) {
        iterationCount++;
        
        // Scan the queue to find a command that can be processed by an available staff member
        // We'll move commands to a temp queue and check each one
        std::queue<std::unique_ptr<Command>> tempQueue;
        std::unique_ptr<Command> commandToProcess = nullptr;
        bool foundProcessableCommand = false;
        
        while (!requestQueue.empty()) {
            auto cmd = std::move(requestQueue.front());
            requestQueue.pop();
            
            // If we already found a command to process, just move this one to temp
            if (foundProcessableCommand) {
                tempQueue.push(std::move(cmd));
                continue;
            }
            
            // Check if this command can be processed
            Command* peekedCmd = cmd.get();
            auto* loggingCmd = dynamic_cast<LoggingCommand*>(peekedCmd);
            if (loggingCmd) {
                peekedCmd = loggingCmd->getInnerCommand();
            }
            
            // Determine command type
            bool isCustomerCommand = (dynamic_cast<FulfillCustomerCommand*>(peekedCmd) != nullptr);
            bool isPlantCareCommand = (dynamic_cast<WaterPlantCommand*>(peekedCmd) != nullptr ||
                                       dynamic_cast<FertilizeCommand*>(peekedCmd) != nullptr ||
                                       dynamic_cast<RemoveWitheredPlantCommand*>(peekedCmd) != nullptr);
            
            // Check if an appropriate handler is available for this specific command type
            bool handlerAvailable = false;
            auto currentStaff = staffChainHead;
            
            while (currentStaff) {
                bool isCashier = (dynamic_cast<Cashier*>(currentStaff.get()) != nullptr);
                bool isGardener = (dynamic_cast<Gardener*>(currentStaff.get()) != nullptr);
                bool isBusy = currentStaff->isBusy();
                
                // Check if this staff can handle this command type and is available
                if (!isBusy) {
                    if ((isCustomerCommand && isCashier) || (isPlantCareCommand && isGardener)) {
                        handlerAvailable = true;
                        break;  // Found an available handler, no need to check more staff
                    }
                }
                
                currentStaff = currentStaff->getSuccessor();
            }
            
            if (handlerAvailable) {
                // Found a command we can process!
                commandToProcess = std::move(cmd);
                foundProcessableCommand = true;
            } else {
                // Can't process this command yet, put it in temp queue
                tempQueue.push(std::move(cmd));
            }
        }
        
        // Restore the temp queue back to requestQueue
        requestQueue = std::move(tempQueue);
        
        if (!foundProcessableCommand) {
            // No commands in the queue can be processed right now
            break;
        }
        
        // Process the command we found
        // If this is a LoggingCommand, set its executed step so the CommandLog
        // records when the command was actually run (separate from queued step).
        if (auto* lc = dynamic_cast<LoggingCommand*>(commandToProcess.get())) {
            lc->setExecutedStep(currentStep);
        }

        // Pass to chain - each staff member checks if they can handle it and if they're not busy
        staffChainHead->handleRequest(std::move(commandToProcess));
    }

    // Cleanup: Handle unprocessed commands remaining in queue
    // Customer commands = customers left (remove and penalize)
    // Plant care commands = retry next step (keep in queue)
    std::vector<std::unique_ptr<Command>> allCommands;
    int customersWhoLeft = 0;
    
    // Extract all remaining commands
    while (!requestQueue.empty()) {
        allCommands.push_back(std::move(requestQueue.front()));
        requestQueue.pop();
    }
    
    // Process each unprocessed command
    for (auto& cmd : allCommands) {
        // Unwrap LoggingCommand to check actual command type
        Command* actualCmd = cmd.get();
        auto* loggingCmd = dynamic_cast<LoggingCommand*>(actualCmd);
        
        // Extract the inner command for type checking
        Command* innerCmd = actualCmd;
        if (loggingCmd) {
            innerCmd = loggingCmd->getInnerCommand();
        }
        
        auto* customerCmd = dynamic_cast<FulfillCustomerCommand*>(innerCmd);
        
        if (customerCmd) {
            // Customer command that wasn't processed = customer left
            customersWhoLeft++;
            
            // Don't re-queue - customer is gone
        } else {
            // Plant care command - re-queue for the NEXT step (they should
            // become eligible in the following step)
            
            // Extract inner command and re-wrap with current step
            if (loggingCmd) {
                auto extractedCmd = loggingCmd->extractInnerCommand();
                // queue for the next step
                auto rewrapped = std::make_unique<LoggingCommand>(std::move(extractedCmd), commandLog, currentStep + 1);
                requestQueue.push(std::move(rewrapped));
            } else {
                // Not wrapped, just re-queue as-is
                requestQueue.push(std::move(cmd));
            }
        }
    }

    if (customersWhoLeft > 0) {
        customersLeftThisStep = customersWhoLeft;
        adjustReputation(-3 * customersWhoLeft);
    } else {
        customersLeftThisStep = 0;
    }

    // Populate per-step UI lists from the commandLog
    // Use the executed-step view for "completed this step" so we capture
    // commands that finished during this step (regardless of when they were queued).
    completedCommandsThisStep = commandLog->completedTextsForExecutedStep(currentStep);
    remainingCommandsAtStepEnd = commandLog->remainingPendingTextsForStep(currentStep);
    
    // Remove customer commands from "remaining" display (they're already counted as "customers left")
    std::vector<std::string> filteredRemaining;
    for (const auto& cmdText : remainingCommandsAtStepEnd) {
        // Only keep plant care commands in "remaining"
        if (cmdText.find("Purchase:") == std::string::npos && 
            cmdText.find("Recommendation") == std::string::npos) {
            filteredRemaining.push_back(cmdText);
        }
    }
    remainingCommandsAtStepEnd = filteredRemaining;
    
    // Advance step counter
    currentStep++;

    // Update phase
    if (currentStep == 5) {
        currentPhase = GamePhase::DAY_END;  // All steps complete - can save/hire
        
        // Deduct daily staff costs at end of day
        const int costperStaff = 80;
        int staffCount = 0;
        auto currentStaff = staffChainHead;
        while (currentStaff) {
            staffCount++;
            currentStaff = currentStaff->getSuccessor();
        }
        int dailyStaffCost = staffCount * costperStaff;
        adjustMoney(-dailyStaffCost);
        
    } else {
        currentPhase = GamePhase::STEP_BREAK;  // Between steps - can plant/view
    }

    return true;
}

void Nursery::addRequest(std::unique_ptr<Command> cmd) {
    if (!cmd) return;
    
    // Duplicate checking: prevent multiple identical commands for the same target
    // Check if a similar command (same type + target) is already pending in queue
    Command* actualCmd = cmd.get();
    auto* loggingCmd = dynamic_cast<LoggingCommand*>(actualCmd);
    if (loggingCmd) {
        actualCmd = loggingCmd->getInnerCommand();
    }
    
    // Get command type info for duplicate checking
    bool isWaterCmd = dynamic_cast<WaterPlantCommand*>(actualCmd) != nullptr;
    bool isFertilizeCmd = dynamic_cast<FertilizeCommand*>(actualCmd) != nullptr;
    bool isRemoveCmd = dynamic_cast<RemoveWitheredPlantCommand*>(actualCmd) != nullptr;
    uint64_t targetId = cmd->getTargetId();
    
    // Only check duplicates for plant-care commands (not customer commands)
    if ((isWaterCmd || isFertilizeCmd || isRemoveCmd) && targetId != 0) {
        // Scan queue for duplicate plant-care commands with same target
        // NOTE: This is a linear scan; for large queues consider a set/map tracking
        std::queue<std::unique_ptr<Command>> tempQueue;
        bool foundDuplicate = false;
        
        while (!requestQueue.empty()) {
            auto& queuedCmd = requestQueue.front();
            
            // Unwrap if needed
            Command* queuedActual = queuedCmd.get();
            auto* queuedLogging = dynamic_cast<LoggingCommand*>(queuedActual);
            if (queuedLogging) {
                queuedActual = queuedLogging->getInnerCommand();
            }
            
            // Check if types match
            bool queuedIsWater = dynamic_cast<WaterPlantCommand*>(queuedActual) != nullptr;
            bool queuedIsFertilize = dynamic_cast<FertilizeCommand*>(queuedActual) != nullptr;
            bool queuedIsRemove = dynamic_cast<RemoveWitheredPlantCommand*>(queuedActual) != nullptr;
            
            // Same command type and target = duplicate
            if (queuedCmd->getTargetId() == targetId &&
                ((isWaterCmd && queuedIsWater) ||
                 (isFertilizeCmd && queuedIsFertilize) ||
                 (isRemoveCmd && queuedIsRemove))) {
                foundDuplicate = true;
            }
            
            tempQueue.push(std::move(requestQueue.front()));
            requestQueue.pop();
        }
        
        // Restore queue
        requestQueue = std::move(tempQueue);
        
        // Skip adding if duplicate found
        if (foundDuplicate) {
            return;
        }
    }
    
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
        // Parse the inventory serialization so we can ensure a Storage group exists
        try {
            auto invJson = nlohmann::json::parse(inventory->serialize());

            bool hasStorage = false;
            if (invJson.contains("components") && invJson["components"].is_array()) {
                for (const auto& comp : invJson["components"]) {
                    if (comp.is_object() && comp.contains("type") && comp["type"].get<std::string>() == "Group") {
                        if (comp.contains("name") && comp["name"].get<std::string>() == "Storage") {
                            hasStorage = true;
                            break;
                        }
                    }
                }
            }

            if (!hasStorage) {
                // Create a temporary empty Storage group (does not modify in-memory inventory)
                auto tempStorage = std::make_shared<Group>("Storage", true);
                // Serialize and append to the components array
                nlohmann::json tempJson = nlohmann::json::parse(tempStorage->serialize());
                if (!invJson.contains("components") || !invJson["components"].is_array()) {
                    invJson["components"] = nlohmann::json::array();
                }
                invJson["components"].push_back(tempJson);
            }

            json << invJson.dump();
        } catch (...) {
            // Fallback to raw serialization on any error
            json << inventory->serialize();
        }
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
        // YEA THIS IS HARD CODED, IT SHOULD BE EASY TO FIX BY USING A  FILTER ITERATOR
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

    // Create command
    auto specPtr = std::make_unique<PlantSpecification>(spec);
    auto command = std::make_unique<FulfillCustomerCommand>(std::move(specPtr), inventory,
                                                            shared_from_this());
    
    addRequest(std::move(command));
}

void Nursery::setupNursery() {
    //Creating default staff chain if it does not exist
    if(!staffChainHead){
        auto cashier = std::make_shared<Cashier>();
        auto gardener = std::make_shared<Gardener>();
        cashier->setSuccessor(gardener);
        staffChainHead = cashier;
    }

    //Registering plant factories (Method unimplemented)
    registerDefaultFactories();

    //Creating the supervisor (owned by Nursery)
    if (!supervisor) {
        supervisor = std::make_shared<NurserySupervisor>(shared_from_this());
    }
    // Attach supervisor to all existing plants
    attachSupervisorToAllExistingPlants();
    
    // Register callback so newly added plants get supervisor attached
    if (inventory && supervisor) {
        auto sup = supervisor; // Capture supervisor in lambda
        inventory->setOnPlantAddedCallback([sup](const std::shared_ptr<Plant>& plant) {
            if (!plant) return;
            // Attach supervisor to all newly added plants. Storage-specific
            // behavior (empty on new setup) is handled during setup/restore.
            plant->attach(sup);
        });
    }

        // Ensure a Storage group exists for a newly setup nursery and is empty.
        if (inventory) {
            auto storage = inventory->findGroupByName("Storage");
            if (!storage) {
                storage = std::make_shared<Group>("Storage", true);
                inventory->add(storage);
            } else {
                // Clear any existing members to ensure new nurseries start with empty storage
                auto members = storage->members();
                for (auto& member : members) {
                    if (member) storage->remove(member);
                }
            }
        }

    //Creating the command log
    if(!commandLog){
        commandLog = std::make_shared<CommandLog>();
    }

    //Init step tracking
    customersLeftThisStep = 0;
    completedCommandsThisStep.clear();
    remainingCommandsAtStepEnd.clear();
}

// Register a small set of default factories so plant creation via UI/customer works
void Nursery::registerDefaultFactories() {
    // Only register if map is empty to avoid overwriting custom factories
    if (!plantFactories.empty()) return;

    try {
        plantFactories["Rose"] = std::make_shared<RoseFactory>();
    } catch (...) {
        // If RoseFactory isn't available for some build configs, ignore
    }
    try { plantFactories["Cactus"] = std::make_shared<CactusFactory>(); } catch(...) {}
    try { plantFactories["Orchid"] = std::make_shared<OrchidFactory>(); } catch(...) {}
    try { plantFactories["SnakePlant"] = std::make_shared<SnakePlantFactory>(); } catch(...) {}
    try { plantFactories["Daisy"] = std::make_shared<DaisyFactory>(); } catch(...) {}
    try { plantFactories["Sunflower"] = std::make_shared<SunflowerFactory>(); } catch(...) {}
    try { plantFactories["Lavender"] = std::make_shared<LavenderFactory>(); } catch(...) {}
    try { plantFactories["Ivy"] = std::make_shared<IvyFactory>(); } catch(...) {}
    try { plantFactories["Succulent"] = std::make_shared<SucculentFactory>(); } catch(...) {}
    try { plantFactories["Tulip"] = std::make_shared<TulipFactory>(); } catch(...) {}
    try { plantFactories["Aloe"] = std::make_shared<AloeFactory>(); } catch(...) {}
    try { plantFactories["Bamboo"] = std::make_shared<BambooFactory>(); } catch(...) {}
    try { plantFactories["Fern"] = std::make_shared<FernFactory>(); } catch(...) {}
    try { plantFactories["Mint"] = std::make_shared<MintFactory>(); } catch(...) {}
    try { plantFactories["Petunia"] = std::make_shared<PetuniaFactory>(); } catch(...) {}
    try { plantFactories["Basil"] = std::make_shared<BasilFactory>(); } catch(...) {}
    try { plantFactories["Marigold"] = std::make_shared<MarigoldFactory>(); } catch(...) {}
}

// Attach the (single) supervisor to all plants currently in inventory
void Nursery::attachSupervisorToAllExistingPlants() {
    if (!supervisor) return;
    auto sup = supervisor; // shared_ptr
    if (!inventory) return;
    auto allPlants = inventory->getAllPlants();
    for (auto& plant : allPlants) {
        if (plant) {
            plant->attach(sup);
        }
    }
}

void Nursery::adjustMoney(double amount) { money += amount; }

void Nursery::adjustReputation(int change) {
    reputation += change;
    // Clamp between 0 and 100
    if (reputation < 0) reputation = 0;
    if (reputation > 100) reputation = 100;
}

void Nursery::postRestoreInit() {
    // Recreate runtime-only systems after a restore from memento
    // 1) Ensure commandLog exists
    if (!commandLog) {
        commandLog = std::make_shared<CommandLog>();
    }

    // 2) Ensure factories are available
    registerDefaultFactories();

    // 3) Create supervisor if missing and attach to plants
    if (!supervisor) {
        supervisor = std::make_shared<NurserySupervisor>(shared_from_this());
    }
    attachSupervisorToAllExistingPlants();
    
    // 4) Register callback so newly added plants get supervisor attached
    if (inventory && supervisor) {
        auto sup = supervisor; // Capture supervisor in lambda
        inventory->setOnPlantAddedCallback([sup](const std::shared_ptr<Plant>& plant) {
            if (!plant) return;
            // Do not attach supervisor to plants stored in Storage groups
            auto owner = plant->getOwner();
            if (owner) {
                auto groupOwner = std::dynamic_pointer_cast<Group>(owner);
                if (groupOwner && groupOwner->getName() == "Storage") {
                    return; // skip attaching for storage plants
                }
            }
            plant->attach(sup);
        });
    }

    // 5) Normalize transient flags (staff busy -> false)
    auto currentStaff = staffChainHead;
    while (currentStaff) {
        currentStaff->setBusy(false);
        currentStaff = currentStaff->getSuccessor();
    }

    // 6) Clear any queued requests (requests are transient)
    while (!requestQueue.empty()) requestQueue.pop();
}

std::shared_ptr<Nursery> Nursery::createAndSetup() {
    auto ptr = std::make_shared<Nursery>();
    ptr->setupNursery();
    return ptr;
}
