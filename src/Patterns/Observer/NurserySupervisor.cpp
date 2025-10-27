#include "../../../include/Patterns/Observer/NurserySupervisor.h"
#include "../../../include/Patterns/Command/WaterPlantCommand.h"
#include "../../../include/Core/Nursery.h"
#include "../../../include/Components/Plant.h"

NurserySupervisor::NurserySupervisor(const std::shared_ptr<Nursery>& nursery) : nursery(nursery) {}

void NurserySupervisor::update(const std::shared_ptr<Subject>& subject) {
    // Cast to Plant to access plant-specific method
    auto plant = std::dynamic_pointer_cast<Plant>(subject);
    if(!plant) 
        return;

    // Lock the weak_pointer(nursery) to get a shared_pointer
    auto nurseryPtr = nursery.lock();
    if(!nurseryPtr) 
        return;

    // call the watercommand method
    if(plant->getWaterLevel() < 50) {
        auto cmd = std::make_unique<WaterPlantCommand>(plant);
        nurseryPtr->addRequest(std::move(cmd));
    }
}
