#include "../../../include/Patterns/Command/RemoveWitheredPlantCommand.h"

#include <memory>
#include <typeinfo>
#include <sstream>

#include "../../../include/Components/Group.h"
#include "../../../include/Components/Plant.h"
#include "../../../include/Patterns/State/Withered.h"

RemoveWitheredPlantCommand::RemoveWitheredPlantCommand(const std::shared_ptr<Plant>& plant,
                                                       const std::shared_ptr<Group>& group)
    : currentStatus(Status::Pending), targetId(0) {
    if (plant) {
        targetPlant = plant;
        targetId = plant->getId();
        plantName = plant->getName();  // Cache the plant name
    } else {
        plantName = "<unknown plant>";
    }
    if (group) {
        parentGroup = group;
        groupName = group->getName();  // Cache the group name
    } else {
        groupName = "<unknown group>";
    }
}

void RemoveWitheredPlantCommand::execute() {
    auto plant = targetPlant.lock();
    auto group = parentGroup.lock();

    if (!plant || !group) {
        currentStatus = Status::Failed;
        return;
    }

    PlantState* state = plant->getState();
    if (state && dynamic_cast<Withered*>(state)) {
        group->remove(plant);
        currentStatus = Status::Completed;
    } else {
        currentStatus = Status::Failed;
    }
}

RemoveWitheredPlantCommand::Status RemoveWitheredPlantCommand::getStatus() const {
    return currentStatus;
}

void RemoveWitheredPlantCommand::setStatus(Status s) { currentStatus = s; }

uint64_t RemoveWitheredPlantCommand::getTargetId() const { return targetId; }

void RemoveWitheredPlantCommand::setTargetId(uint64_t id) { targetId = id; }

std::string RemoveWitheredPlantCommand::toString() const {
    std::ostringstream out;

    if (currentStatus == Status::Completed){
        out << "Removed withered " << plantName << " from " << groupName;
    }
    else if(currentStatus == Status::Pending){
        out << "Need to remove withered " << plantName << " from " << groupName;
    }
    else if(currentStatus == Status::Failed){
        out << "Failed to remove " << plantName;
    }
    else{
        out << "Remove withered " << plantName;
    }

    return out.str();
}
