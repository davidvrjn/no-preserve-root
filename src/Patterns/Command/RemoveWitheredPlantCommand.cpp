#include "../../../include/Patterns/Command/RemoveWitheredPlantCommand.h"

#include <memory>
#include <typeinfo>

#include "../../../include/Components/Group.h"
#include "../../../include/Components/Plant.h"
#include "../../../include/Patterns/State/Withered.h"

RemoveWitheredPlantCommand::RemoveWitheredPlantCommand(const std::shared_ptr<Plant>& plant,
                                                       const std::shared_ptr<Group>& group)
    : currentStatus(Status::Pending), targetId(0) {
    if (plant) {
        targetPlant = plant;
        targetId = plant->getId();
    }
    if (group) {
        parentGroup = group;
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
    if (state && typeid(*state) == typeid(Withered)) {
        plant->detachAllObservers();
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
