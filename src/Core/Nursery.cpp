#include "../../include/Core/Nursery.h"

#include <algorithm>
#include <map>
#include <random>

#include "../../include/Actors/Cashier.h"
#include "../../include/Actors/Customer.h"
#include "../../include/Actors/Gardener.h"
#include "../../include/Components/PlantAttributes.h"
#include "../../include/Patterns/Builder/ConcretePlantSpecificationBuilder.h"
#include "../../include/Patterns/Command/Command.h"
#include "../../include/Patterns/Command/FulfillCustomerCommand.h"
#include "../../include/Patterns/Command/WaterPlantCommand.h"
#include "../../include/Patterns/Memento/Memento.h"

Nursery::Nursery()
    : currentDay(0),
      money(1000.0),
      reputation(50)  // Start at 50/100 (neutral)
{}

Nursery::~Nursery() = default;

void Nursery::runSimulation() {
    // for future implementation
    // essentially there will be 5 steps for each day
    // wherein customers are spawned, staff process requests, etc.
    // plants processes of "growing" are updated before the steps
    // this implies that are observer created commands (plant management) are added to the queue
    // immediately at the start of a new day whereas customer-related commands are added to the
    // queue as they come in per step the steps are basically for queue processing the staff is
    // "busy" during a step and a command takes 1 step to complete
}

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

void Nursery::processRequestQueue()
{
    // Process commands in the queue by delegating to the staff chain
    // Commands that cannot be handled (all appropriate staff busy) remain in queue
    
    if(!staffChainHead)
    {
        // No staff available, leave all commands in queue
        return;
    }
    
    if(requestQueue.empty())
    {
        return;  // Nothing to process
    }
    
    // Create a temporary queue to hold commands that couldn't be processed
    std::queue<std::unique_ptr<Command>> deferredCommands;
    
    // Process all commands currently in the queue
    size_t initialSize = requestQueue.size();
    
    for(size_t i = 0; i < initialSize; ++i)
    {
        if(requestQueue.empty())
        {
            break;
        }
        
        auto cmd = std::move(requestQueue.front());
        requestQueue.pop();
        
        if(!cmd)
        {
            continue;  // Skip null commands
        }
        
        // Check if any staff member can handle this command
        if(canStaffHandleCommand(cmd.get()))
        {
            // At least one non-busy staff member can handle this command type
            // Delegate to the staff chain
            staffChainHead->handleRequest(std::move(cmd));
        }
        else
        {
            // No available staff can handle this command right now
            // Defer it for the next processing cycle
            deferredCommands.push(std::move(cmd));
        }
    }
    
    // Put deferred commands back into the main queue
    while(!deferredCommands.empty())
    {
        requestQueue.push(std::move(deferredCommands.front()));
        deferredCommands.pop();
    }
}

bool Nursery::canStaffHandleCommand(const Command* cmd) const
{
    // Check if any staff member in the chain can handle this command
    // and is not currently busy
    
    if(!cmd || !staffChainHead)
    {
        return false;
    }
    
    // Determine command type
    bool isWaterCommand = dynamic_cast<const WaterPlantCommand*>(cmd) != nullptr;
    bool isCustomerCommand = dynamic_cast<const FulfillCustomerCommand*>(cmd) != nullptr;
    
    if(!isWaterCommand && !isCustomerCommand)
    {
        return false;  // Unknown command type
    }
    
    // Walk through the staff chain manually
    // Since we don't have direct access to the chain, we need to check each staff type
    
    // Check if we have gardeners for water commands
    if(isWaterCommand)
    {
        // Try to cast staffChainHead to Gardener
        auto gardener = std::dynamic_pointer_cast<Gardener>(staffChainHead);

        if(gardener && !gardener->isBusy())
        {
            return true;
        }
        
        // Check if there are more gardeners in the chain via successor
        // Since Staff doesn't expose getSuccessor(), we assume the chain is properly set up
        // and rely on the fact that if the head can't handle it, successors might
        // For now, we'll return true if we have any staff chain at all
        // The actual handling will be determined by the chain itself
        return true;  // Let the chain try to handle it
    }
    
    // Check if we have cashiers for customer commands
    if(isCustomerCommand)
    {
        auto cashier = std::dynamic_pointer_cast<Cashier>(staffChainHead);

        if(cashier && !cashier->isBusy())
        {
            return true;
        }
        
        // Similar logic as above - let the chain try
        return true;
    }
    
    return false;
}

void Nursery::setupNursery() {}

void Nursery::adjustMoney(double amount) { money += amount; }

void Nursery::adjustReputation(int change) {
    reputation += change;
    // Clamp between 0 and 100
    if (reputation < 0) reputation = 0;
    if (reputation > 100) reputation = 100;
}