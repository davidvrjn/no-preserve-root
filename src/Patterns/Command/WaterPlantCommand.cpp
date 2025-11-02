#include "../../../include/Patterns/Command/WaterPlantCommand.h"

#include <memory>
#include <sstream>

#include "../../../include/Components/Plant.h"
#include "../../../include/Components/Group.h"

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

std::string WaterPlantCommand::toString() const {
    std::ostringstream out;
    // Try to lock plant to query name and owner group
    auto plant = targetPlant.lock();
    std::string plantName = "<unknown>";
    std::string ownerName = "<unknown group>";
    if (plant) {
        plantName = plant->getName();
        auto owner = plant->getOwner();
        if (owner) ownerName = owner->getName();
    }

    if (currentStatus == Status::Completed) {
        out << "Watered " << plantName << " in " << ownerName;
    } else if (currentStatus == Status::Pending) {
        out << "Need to water " << plantName << " in " << ownerName;
    } else if (currentStatus == Status::Failed) {
        out << "Failed to water " << plantName;
    } else {
        out << "WaterPlantCommand";
    }

    return out.str();
}