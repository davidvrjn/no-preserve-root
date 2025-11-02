/**
 * @file NurserySupervisor.cpp
 * @brief Implementation of the NurserySupervisor observer class
 * @version 0.1
 * @date 2025-11-01
 *
 * @copyright Copyright (c) 2025
 *
 * This file implements the NurserySupervisor class which monitors plants in the
 * nursery and automatically creates watering command when plants needs water
 */

#include "../../../include/Patterns/Observer/NurserySupervisor.h"

#include "../../../include/Components/Group.h"
#include "../../../include/Components/Plant.h"
#include "../../../include/Core/Nursery.h"
#include "../../../include/Patterns/Command/AddToStorageCommand.h"
#include "../../../include/Patterns/Command/FertilizeCommand.h"
#include "../../../include/Patterns/Command/RemoveWitheredPlantCommand.h"
#include "../../../include/Patterns/Command/WaterPlantCommand.h"
#include "../../../include/Patterns/State/Mature.h"
#include "../../../include/Patterns/State/Withered.h"
#include "../../../include/Patterns/State/Withering.h"

/**
 * @brief Construct a new Nursery Supervisor:: Nursery Supervisor object
 *
 * @param nursery Shared pointer to the Nursery being supervised.
 *                 Stored as a weak_ptr to avoid circular ownership.
 *
 * @note The nursery is stored as a weak_ptr to prevent strong reference cycles
 *      that could cause memory leaks in the observer pattern.
 */
NurserySupervisor::NurserySupervisor(const std::shared_ptr<Nursery>& nursery) : nursery(nursery) {}

/**
 * @brief Updates the supervisor when a plant's state changes (Observer pattern)
 *
 * This method is called automatically when an observed plant notifies its observers.
 * It checks the plant's water leve; and creates a WaterCommand if the water
 * level falls below 50%.
 *
 *
 * @param subject  The Subject (plant) that triggered the notification.
 *                 Must be castable to plant Type.
 *
 * @details Operation flow :
 *          1. attempts to cast the subject to a Plant
 *          2. Locks the weak_ptr to get access to the Nursery
 *          3. Checks if plant's water level is below 50%
 *          4. IF low, creates and queues a WaterPlantCommand
 *
 * @warning If the nursery has been destroyed (weak_ptr exoired), this method
 *          return early withoyt creating commands
 *
 * @see WaterPlantCommand, Plant::getWaterLevel(), Nursery::addRequest()
 */
void NurserySupervisor::update(const std::shared_ptr<Subject>& subject) {
    // Cast to Plant to access plant-specific method
    auto plant = std::dynamic_pointer_cast<Plant>(subject);
    if (!plant) return;

    // Lock the weak_pointer(nursery) to get a shared_pointer
    auto nurseryPtr = nursery.lock();
    if (!nurseryPtr) return;

    // Determine current state of the plant
    auto state = plant->getState();
    if (!state) return;  // Safety: no state = can't process

    // call the watercommand method
    if (plant->getWaterLevel() < 50 && !dynamic_cast<Mature*>(state)) {
        auto cmd = std::make_unique<WaterPlantCommand>(plant);
        nurseryPtr->addRequest(std::move(cmd));
    }

    if (dynamic_cast<Withering*>(state)) {
        // FertilizeCommand requires the nursery to deduct cost; pass nurseryPtr
        auto cmd = std::make_unique<FertilizeCommand>(plant, nurseryPtr);
        nurseryPtr->addRequest(std::move(cmd));
    } else if (dynamic_cast<Withered*>(state)) {
        // RemoveWitheredPlantCommand requires the parent group
        // Safety: only create command if plant has a valid owner
        auto owner = plant->getOwner();
        if (owner) {
            auto cmd = std::make_unique<RemoveWitheredPlantCommand>(plant, owner);
            nurseryPtr->addRequest(std::move(cmd));
        }
        // Note: If plant has no owner, it can't be removed from a group
        // This shouldn't happen in practice but we guard against it
    } else if (dynamic_cast<Mature*>(state)) {
        // When plant matures, move it to storage
        auto cmd = std::make_unique < AddToStorageCommand(plant, nurseryPtr->getInventory());
        nurseryPtr->addRequest(std::move(cmd));
    }
}
