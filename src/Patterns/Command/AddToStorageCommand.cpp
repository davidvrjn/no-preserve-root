//Same as its respective header file fix the imports at a later time
#include "AddToStorageCommand.h"

AddToStorageCommand::AddToStorageCommand(
    const std::shared_ptr<InventoryComponent>& source,
    const std::shared_ptr<InventoryComponent>& target,
    const std::shared_ptr<Plant>& plant
    ) 
    : source(source), target(target), plant(plant)
{
    if(target){
        setTargetId(target->getId());
    }else{
        setTargetId(0);
    }
}

