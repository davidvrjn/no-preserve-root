//Same as its respective header file fix the imports at a later time
#include "AddToStorageCommand.h"

AddToStorageCommand::AddToStorageCommand(
    const std::shared_ptr<InventoryComponent>& source,
    const std::shared_ptr<InventoryComponent>& target,
    const std::shared_ptr<Plant>& plant
    ) 
    : source(source), target(target), plant(plant), currentStatus(Status::Pending), targetId(0)
{
    //If the target Id exsists store it
    if(target){
        setTargetId(target->getId());
    }
}

void AddToStorageCommand::execute() {
    //Check if everything is good
    if(!source || !target || !plant){
        setStatus(Status::Failed);
        return;
    }

    //remove the plant in question
    auto toGroup = std::dynamic_pointer_cast<Group>(source);

    if(!toGroup){
        setStatus(Status::Failed);
        return;
    }

    toGroup->remove(plant);

    //Add the plant to inventory
    auto destinationGroup = std::dynamic_pointer_cast<Group>(target);
    if(!destinationGroup){
        setStatus(Status::Failed);
        return;
    }

    destinationGroup->add(plant);
    setStatus(Status::Completed);
}

AddToStorageCommand::Status AddToStorageCommand::getStatus() const {
    return currentStatus;
}

void AddToStorageCommand::setStatus(Status s) {
    currentStatus = s;
}

uint64_t AddToStorageCommand::getTargetId() const {
    return targetId;
}

void AddToStorageCommand::setTargetId(uint64_t id){
    targetId = id;
}