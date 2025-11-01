#include "../../../include/Patterns/Observer/NurserySupervisor.h"

#include <fstream>

#include "../../../include/Components/Plant.h"
#include "../../../include/Core/Nursery.h"
#include "../../../include/Patterns/Command/FertilizeCommand.h"
#include "../../../include/Patterns/Command/RemoveWitheredPlantCommand.h"
#include "../../../include/Patterns/Command/WaterPlantCommand.h"
#include "../../../include/Patterns/State/Withering.h"
#include "../../../include/Patterns/State/Withered.h"
#include "../../../include/Patterns/State/Mature.h"
#include "../../../include/Components/Group.h"

NurserySupervisor::NurserySupervisor(const std::shared_ptr<Nursery>& nursery) : nursery(nursery) {}

void NurserySupervisor::update(const std::shared_ptr<Subject>& subject) {
    // Cast to Plant to access plant-specific method
    auto plant = std::dynamic_pointer_cast<Plant>(subject);
    if (!plant) return;

    // Lock the weak_pointer(nursery) to get a shared_pointer
    auto nurseryPtr = nursery.lock();
    if (!nurseryPtr) return;

    // DEBUG: Log that supervisor was notified
    std::ofstream logFile("debug.log", std::ios::app);
    logFile << "   [SUPERVISOR] Notified about plant, water=" << plant->getWaterLevel();
    
    auto state = plant->getState();
    if (state) {
        if (dynamic_cast<Withering*>(state)) logFile << " state=Withering";
        else if (dynamic_cast<Withered*>(state)) logFile << " state=Withered";
        else if (dynamic_cast<Mature*>(state)) logFile << " state=Mature";
        else logFile << " state=Other";
    }
    logFile << "\n";
    logFile.close();

    // call the watercommand method
    if (plant->getWaterLevel() < 50) {
        auto cmd = std::make_unique<WaterPlantCommand>(plant);
        nurseryPtr->addRequest(std::move(cmd));
    }

    // Determine current state of the plant
    if (!state) return; // Safety: no state = can't process

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
        // Mature plants should be moved to storage
        // For now, we'll just skip (no command created)
        // When ready, create: auto cmd = std::make_unique<AddToStorageCommand>(plant, owner);
    }
}
