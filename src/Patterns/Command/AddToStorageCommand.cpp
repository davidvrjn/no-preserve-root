#include "../../../include/Patterns/Command/AddToStorageCommand.h"

#include <memory>
#include <sstream>
#include <typeinfo>

#include "../../../include/Components/Group.h"
#include "../../../include/Components/Plant.h"
#include "../../../include/Core/Inventory.h"
#include "../../../include/Patterns/State/Mature.h"

AddToStorageCommand::AddToStorageCommand(const std::shared_ptr<Plant>& plant,
                                         const std::shared_ptr<Inventory>& inv)
    : currentStatus(Status::Pending), targetId(0) {
    if (plant) {
        targetPlant = plant;
        targetId = plant->getId();
    }
    if (inv) {
        inventory = inv;
    }
}

void AddToStorageCommand::execute() {
    auto plant = targetPlant.lock();
    auto inv = inventory.lock();

    if (!plant || !inv) {
        currentStatus = Status::Failed;
        return;
    }

    // Verify plant is in Mature state
    PlantState* state = plant->getState();
    if (!state || typeid(*state) != typeid(Mature)) {
        currentStatus = Status::Failed;
        return;
    }

    // Get current owner (the plot/group the plant is currently in)
    auto currentOwner = plant->getOwner();
    if (!currentOwner) {
        // Plant has no owner, can't move it
        currentStatus = Status::Failed;
        return;
    }

    // Find the Storage group
    auto storage = inv->findGroupByName("Storage");
    if (!storage) {
        // Storage group doesn't exist
        currentStatus = Status::Failed;
        return;
    }

    // Move plant from current owner to Storage
    // Group::add() handles the auto-move logic (removes from previous owner)
    storage->add(plant);
    currentStatus = Status::Completed;
}

AddToStorageCommand::Status AddToStorageCommand::getStatus() const { return currentStatus; }

void AddToStorageCommand::setStatus(Status s) { currentStatus = s; }

uint64_t AddToStorageCommand::getTargetId() const { return targetId; }

void AddToStorageCommand::setTargetId(uint64_t id) { targetId = id; }