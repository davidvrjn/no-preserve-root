#include "../../../include/Patterns/Command/WaterPlantCommand.h"


WaterPlantCommand::WaterPlantCommand(const std::shared_ptr<Plant>& plant) {
    if (plant) {
        targetPlant = plant;
        targetId = plant->getName().empty() ? 0 : reinterpret_cast<uint64_t>(plant.get()); //Okay convert the pointer address to an ID
    }
    currentStatus = Status::Pending;
}

void WaterPlantCommand::execute() 
{
    auto plant = targetPlant.lock();
    if(plant){
        plant->water();
        currentStatus = Status::Completed;
    }
}

std::string WaterPlantCommand::serialize() const { return std::string(); }

void WaterPlantCommand::deserialize(const std::string& data) { (void)data; }

WaterPlantCommand::Status WaterPlantCommand::getStatus() const 
{ 
    return currentStatus;
}

void WaterPlantCommand::setStatus(Status s) {
    currentStatus = s;
}


uint64_t WaterPlantCommand::getTargetId() const { 
    return targetId;
}

void WaterPlantCommand::setTargetId(uint64_t id) {
    targetId = id;
}