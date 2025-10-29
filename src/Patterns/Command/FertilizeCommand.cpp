#include "../../../include/Patterns/Command/FertilizeCommand.h"

#include <memory>

#include "../../../include/Components/Plant.h"
#include "../../../include/Core/Nursery.h"

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

void FertilizeCommand::execute() {
    // just for now
    currentStatus = Status::Failed;

    // basically
    // nur->adjustMoney(-fertCost);
    // plant->fertilize(); (this will set health to 20 and restore state)
}

FertilizeCommand::Status FertilizeCommand::getStatus() const { return currentStatus; }

void FertilizeCommand::setStatus(Status s) { currentStatus = s; }

uint64_t FertilizeCommand::getTargetId() const { return targetId; }

void FertilizeCommand::setTargetId(uint64_t id) { targetId = id; }
