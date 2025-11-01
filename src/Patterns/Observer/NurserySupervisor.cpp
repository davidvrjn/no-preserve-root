#include "../../../include/Patterns/Observer/NurserySupervisor.h"

#include "../../../include/Components/Plant.h"
#include "../../../include/Core/Nursery.h"
#include "../../../include/Patterns/Command/WaterPlantCommand.h"

/**
 * @brief Construct a new Nursery Supervisor:: Nursery Supervisor object
 * 
 * @param nursery Shared pointer to the nursery being supervised.
 * 
 * The supervisor uses a weak pointer to the nursery to prevent circular
 * references while maintaining the ability to queue watering commands.
 */
NurserySupervisor::NurserySupervisor(const std::shared_ptr<Nursery>& nursery) : nursery(nursery) {}

/**
 * @brief Updates the supervisor when a plant's state changes.
 * 
 * This method is called when an observed plant notifies its observers of a state change.
 * The supervisor checks the plant's water level and automatically queues a watering
 * command if the level drops below 50.
 * 
 * @param subject Shared pointer to the subject (plant) that triggered the update.
 * 
 * The method performs the following operations:
 * 1. Casts the subject to a Plant pointer
 * 2. Locks the weak_ptr to verify the nursery still exists
 * 3. Checks if the plant's water level is below 50
 * 4. If low, creates and queues a WaterPlantCommand to the nursery's request queue
 * 
 * @note The method returns early if:
 *       - The subject cannot be cast to a Plant
 *       - The nursery no longer exists (weak_ptr expired)
 *       - The plant's water level is 50 or above
 */
void NurserySupervisor::update(const std::shared_ptr<Subject>& subject) {
    // Cast to Plant to access plant-specific method
    auto plant = std::dynamic_pointer_cast<Plant>(subject);
    if (!plant) return;

    // Lock the weak_pointer(nursery) to get a shared_pointer
    auto nurseryPtr = nursery.lock();
    if (!nurseryPtr) return;

    // call the watercommand method
    if (plant->getWaterLevel() < 50) {
        auto cmd = std::make_unique<WaterPlantCommand>(plant);
        nurseryPtr->addRequest(std::move(cmd));
    }
}
