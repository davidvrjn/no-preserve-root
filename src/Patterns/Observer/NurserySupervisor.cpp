#include "../../../include/Patterns/Observer/NurserySupervisor.h"

#include "../../../include/Components/Plant.h"
#include "../../../include/Core/Nursery.h"
#include "../../../include/Patterns/Command/FertilizeCommand.h"
#include "../../../include/Patterns/Command/RemoveWitheredPlantCommand.h"
#include "../../../include/Patterns/Command/WaterPlantCommand.h"
#include "../../../include/Patterns/State/Withering.h"
#include "../../../include/Patterns/State/Withered.h"
#include "../../../include/Components/Group.h"

NurserySupervisor::NurserySupervisor(const std::shared_ptr<Nursery>& nursery) : nursery(nursery) {}

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

    // Determine current state of the plant
    auto state = plant->getState();

    auto owner = std::dynamic_pointer_cast<Group>(plant->getOwner());

    if (dynamic_cast<Withering*>(state)) {
        auto cmd = std::make_unique<FertilizeCommand>(plant, nurseryPtr);
        nurseryPtr->addRequest(std::move(cmd));
    } else if (dynamic_cast<Withered*>(state)) {
        auto cmd = std::make_unique<RemoveWitheredPlantCommand>(plant, owner);
        nurseryPtr->addRequest(std::move(cmd));
    }
}
