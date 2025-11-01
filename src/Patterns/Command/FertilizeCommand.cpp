#include "../../../include/Patterns/Command/FertilizeCommand.h"

#include <memory>
#include <string>
#include <sstream>

#include "../../../include/Components/Plant.h"
#include "../../../include/Core/Nursery.h"
#include "../../../include/Components/Group.h"

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

FertilizeCommand::Status FertilizeCommand::getStatus() const { return currentStatus; }

void FertilizeCommand::setStatus(Status s) { currentStatus = s; }

uint64_t FertilizeCommand::getTargetId() const { return targetId; }

void FertilizeCommand::setTargetId(uint64_t id) { targetId = id; }

std::string FertilizeCommand::toString() const{
    auto plant = targetPlant.lock();
    if(!plant){
        return "Fertilize could not find plant";
    }

    std::ostringstream out;
    std::string plantName = plant->getName();
    auto owner = plant->getOwner();
    std::string ownerName = owner ? owner->getName() : "<unknown group>";

    if (currentStatus == Status::Completed){
        out << "Fertilized " << plantName << " in " << ownerName << " ( - R30)";
    }
    else if(currentStatus == Status::Pending){
        out << "Need to fertilize " << plantName << " in " << ownerName;
    }
    else if(currentStatus == Status::Failed){
        out << "Failed to fertilize " << plantName;
    }
    else{
        out << "Fertilize " << plantName;
    }

    return out.str();
}
