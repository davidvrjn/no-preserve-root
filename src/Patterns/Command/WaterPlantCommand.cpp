#include "../../../include/Patterns/Command/WaterPlantCommand.h"

#include <memory>

#include "../../../include/Components/Plant.h"

WaterPlantCommand::WaterPlantCommand(const std::shared_ptr<Plant>& plant)
    : currentStatus(Status::Pending), targetId(0) {
    if (plant) {
        targetPlant = plant;
        targetId = plant->getId();  // Use the plant's actual ID
    }
}

void WaterPlantCommand::execute() {
    auto plant = targetPlant.lock();
    if (plant) {
        plant->water();
        currentStatus = Status::Completed;
    } else {
        // Plant no longer exists (weak_ptr expired)
        currentStatus = Status::Failed;
    }
}

WaterPlantCommand::Status WaterPlantCommand::getStatus() const { return currentStatus; }

void WaterPlantCommand::setStatus(Status s) { currentStatus = s; }

uint64_t WaterPlantCommand::getTargetId() const { return targetId; }

void WaterPlantCommand::setTargetId(uint64_t id) { targetId = id; }