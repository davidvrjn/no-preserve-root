#include "../../../include/Patterns/Command/FertilizeCommand.h"

#include <memory>
#include <string>

#include "../../../include/Components/Plant.h"
#include "../../../include/Core/Nursery.h"

/**
 * @brief Constructs a FertilizeCommand for a specific plant.
 * 
 * The constructor stores weak pointers to prevent circular references
 * and sets the initial status to Pending.
 * 
 * @param plant Shared pointer to the plant to be fertilized.
 * @param nur  Shared pointer to the nursery that owns the plant.
 */
FertilizeCommand::FertilizeCommand(const std::shared_ptr<Plant>& plant,
                                   const std::shared_ptr<Nursery>& nur)
    : currentStatus(Status::Pending), targetId(0) {
    if (plant) {
        targetPlant = plant;
        targetId = plant->getId();
    }
    if (nur) {
        nursery = nur;
    }
}

/**
 * @brief Executes the fertilize command on the target plant.
 * 
  * This method performs the following operations:
 * 1. Validates that both the plant and nursery still exist
 * 2. Checks if the nursery has sufficient funds (fertCost)
 * 3. Deducts the fertilizer cost from the nursery's budget
 * 4. Applies fertilizer to the plant (sets health to 20, restores from Withering state)
 * 5. Updates the command status to Completed on success
 * 
 * The command will fail and set status to Failed if:
 * - The plant no longer exists (weak_ptr expired)
 * - The nursery no longer exists (weak_ptr expired)
 * - Insufficient funds in the nursery
 * - Any exception occurs during fertilization (cost is refunded) 
 */
void FertilizeCommand::execute()
{
    // just for now
    //currentStatus = Status::Failed;

    // basically
    // nur->adjustMoney(-fertCost);
    // plant->fertilize(); (this will set health to 20 and restore state)

    // Try to lock the weak_ptr to get shared_ptrs
    auto plant = targetPlant.lock();
    auto nur = nursery.lock();
    
    if(!plant)
    {
        // Plant no longer exists
        currentStatus = Status::Failed;
        return;
    }
    
    if(!nur)
    {
        // Nursery no longer exists
        currentStatus = Status::Failed;
        return;
    }
    
    // Check if nursery has enough money for fertilization
    if(nur->getMoney() < fertCost)
    {
        // Not enough money to buy fertilizer
        currentStatus = Status::Failed;
        return;
    }
    
    try
    {
        // Deduct fertilizer cost from nursery
        nur->adjustMoney(-fertCost);
        
        // Apply fertilizer to the plant
        // This sets health to 20 and restores state from Withering
        plant->fertilize();
        
        // Mark command as completed
        currentStatus = Status::Completed;
        
    }
    catch(...)
    {
        // If fertilization fails for any reason, refund the cost
        nur->adjustMoney(fertCost);
        currentStatus = Status::Failed;
    }
}

/**
 * @brief Gets the current status of the fertilize command.
 * 
 * @return FertilizeCommand::Status The current status (Pending, Completed, or Failed).
 */
FertilizeCommand::Status FertilizeCommand::getStatus() const { return currentStatus; }

/**
 * @brief Sets the status of the fertilize command.
 * 
 * @param s  The new status to set.
 */
void FertilizeCommand::setStatus(Status s) { currentStatus = s; }

/**
 * @brief Gets the ID of the target plant.
 * 
 * @return uint64_t The unique identifier of the plant to be fertilized.
 */
uint64_t FertilizeCommand::getTargetId() const { return targetId; }

/**
 * @brief Sets the target plant ID for this command.
 * 
 * @param id The unique identifier of the plant to be fertilized.
 */
void FertilizeCommand::setTargetId(uint64_t id) { targetId = id; }
