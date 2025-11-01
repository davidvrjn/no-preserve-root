#include "../../../include/Patterns/Command/WaterPlantCommand.h"

#include <memory>

#include "../../../include/Components/Plant.h"

/**
 * @brief Construct a new Water Plant Command:: Water Plant Command object
 * 
 * @param plant Shared pointer to the plant to be watered.
 * 
 * The constructor stores a weak pointer to the plant to prevent circular
 * references and sets the initial status to Pending. The target ID is
 * extracted from the plant's actual ID.
 */
WaterPlantCommand::WaterPlantCommand(const std::shared_ptr<Plant>& plant)
    : currentStatus(Status::Pending), targetId(0) {
    if (plant) {
        targetPlant = plant;
        targetId = plant->getId();  // Use the plant's actual ID
    }
}

/**
 * @brief Executes the water command on the target plant.
 * 
 * This method performs the following operations:
 * 1. Attempts to lock the weak_ptr to get a shared_ptr to the plant
 * 2. If the plant exists, calls the plant's water() method
 * 3. Updates the command status to Completed on success
 * 
 * The command will fail and set status to Failed if:
 * - The plant no longer exists (weak_ptr expired)
 * 
 * On success, the plant is watered and may restore health or change state
 * based on the plant's current condition and state pattern implementation.
 */
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

/**
 * @brief Gets the current status of the water command.
 * 
 * @return WaterPlantCommand::Status The current status (Pending, Completed, or Failed).
 */
WaterPlantCommand::Status WaterPlantCommand::getStatus() const { return currentStatus; }

/**
 * @brief Sets the status of the water command.
 * 
 * @param s The new status to set.
 */
void WaterPlantCommand::setStatus(Status s) { currentStatus = s; }

/**
 * @brief  Gets the ID of the target plant.
 * 
 * @return uint64_t The unique identifier of the plant to be watered.
 */
uint64_t WaterPlantCommand::getTargetId() const { return targetId; }

/**
 * @brief Sets the target plant ID for this command.
 * 
 * @param id The unique identifier of the plant to be watered.
 */
void WaterPlantCommand::setTargetId(uint64_t id) { targetId = id; }