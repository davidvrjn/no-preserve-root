#pragma once
#include <memory>

#include "Command.h"

class Plant;
class Group;

class RemoveWitheredPlantCommand : public Command {
   private:
    std::weak_ptr<Plant> targetPlant;
    std::weak_ptr<Group> parentGroup;
    Status currentStatus;
    uint64_t targetId = 0;

   public:
    RemoveWitheredPlantCommand(const std::shared_ptr<Plant>& plant,
                               const std::shared_ptr<Group>& group);
    ~RemoveWitheredPlantCommand() override = default;

    void execute() override;

    Status getStatus() const override;
    void setStatus(Status s) override;
    uint64_t getTargetId() const override;
    void setTargetId(uint64_t id) override;
    std::string toString() const;
};
