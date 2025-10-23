#include "../../../include/Patterns/Observer/NurserySupervisor.h"
#include "../../../include/Core/Nursery.h"
#include "../../../include/Patterns/Command/WaterPlantCommand.h"
#include "../../../include/Patterns/Command/WaterPlantCommand.h"
#include "../../../include/Components/Plant.h"

NurserySupervisor::NurserySupervisor(const std::shared_ptr<Nursery>& nursery) : nursery(nursery) {}

void NurserySupervisor::update(const std::shared_ptr<Subject>& subject) {
    auto plant = std::dynamic_pointer_cast<Plant>(subject);
    if (!plant) return; 

    auto nurseryPtr = nursery.lock();
    if (!nurseryPtr) return;  

    if (plant->getWaterLevel() < plant->getWaterConsumption()) {
        nurseryPtr->addRequest(std::make_unique<WaterPlantCommand>(plant));
    }
}
