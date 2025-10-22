#include "../../../include/Patterns/Command/WaterPlantCommand.h"

WaterPlantCommand::WaterPlantCommand(const std::shared_ptr<Plant>& plant) { (void)plant; }

void WaterPlantCommand::execute() {}

std::string WaterPlantCommand::serialize() const { return std::string(); }
void WaterPlantCommand::deserialize(const std::string& data) { (void)data; }
WaterPlantCommand::Status WaterPlantCommand::getStatus() const { return Status::Pending; }
void WaterPlantCommand::setStatus(Status s) { (void)s; }
uint64_t WaterPlantCommand::getTargetId() const { return 0; }
void WaterPlantCommand::setTargetId(uint64_t id) { (void)id; }
