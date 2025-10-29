
#pragma once
#include <memory>

#include "Command.h"

// Forward declaration
class Plant;

/**
 * @class WaterPlantCommand
 * @brief A concrete Command for watering a specific plant.
 *
 * This object holds all the context needed to perform its action, namely a
 * (non-owning) weak reference to the Plant that needs to be watered. Its execute() method
 * will attempt to lock the weak_ptr and call water() if the plant still exists.
 */
class WaterPlantCommand : public Command {
   private:
    std::weak_ptr<Plant> targetPlant;  // Non-owning reference; may be expired.
    Status currentStatus;
    uint64_t targetId = 0;

   public:
    WaterPlantCommand(const std::shared_ptr<Plant>& plant);
    ~WaterPlantCommand() override = default;

    void execute() override;

    Status getStatus() const override;
    void setStatus(Status s) override;
    uint64_t getTargetId() const override;
    void setTargetId(uint64_t id) override;
};

