#include "../../../include/Patterns/Command/RemoveWitheredPlantCommand.h"

#include <memory>
#include <typeinfo>

#include "../../../include/Components/Group.h"
#include "../../../include/Components/Plant.h"
#include "../../../include/Patterns/State/Withered.h"

/**
 * @brief Construct a new Remove Withered Plant Command:: Remove Withered Plant Command object
 * 
 * @param plant Shared pointer to the plant to be removed if withered.
 * @param group Shared pointer to the parent group containing the plant.
 * 
 * The constructor stores weak pointers to prevent circular references
 * and sets the initial status to Pending.
 */
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

/**
 * @brief Executes the command to remove a withered plant from its group.
 * 
 * This method performs the following operations:
 * 1. Validates that both the plant and group still exist
 * 2. Checks if the plant's current state is Withered
 * 3. If withered, removes the plant from its parent group
 * 4. Updates the command status based on success or failure
 * 
 * The command will fail and set status to Failed if:
 * - The plant no longer exists (weak_ptr expired)
 * - The parent group no longer exists (weak_ptr expired)
 * - The plant's state is not Withered
 * 
 * On success, the plant is removed from the group and status is set to Completed.
 * 
 * @note This command only removes plants that are in the Withered state.
 *       Healthy or withering plants will not be removed.
 */
void RemoveWitheredPlantCommand::execute() {
    auto plant = targetPlant.lock();
    auto group = parentGroup.lock();

    if (!plant || !group) {
        currentStatus = Status::Failed;
        return;
    }

    PlantState* state = plant->getState();
    if (state && typeid(*state) == typeid(Withered)) {
        group->remove(plant);
        currentStatus = Status::Completed;
    } else {
        currentStatus = Status::Failed;
    }
}

/**
 * @brief Gets the current status of the remove command.
 * 
 * @return RemoveWitheredPlantCommand::Status The current status (Pending, Completed, or Failed).
 */
RemoveWitheredPlantCommand::Status RemoveWitheredPlantCommand::getStatus() const {
    return currentStatus;
}

/**
 * @brief Sets the status of the remove command.
 * 
 * @param s The new status to set.
 */
void RemoveWitheredPlantCommand::setStatus(Status s) { currentStatus = s; }

/**
 * @brief 
 * 
 * @return uint64_t 
 */
uint64_t RemoveWitheredPlantCommand::getTargetId() const { return targetId; }

/**
 * @brief Sets the target plant ID for this command.
 * 
 * @param id The unique identifier of the plant to be removed.
 */
void RemoveWitheredPlantCommand::setTargetId(uint64_t id) { targetId = id; }
