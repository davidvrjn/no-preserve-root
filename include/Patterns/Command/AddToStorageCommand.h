#pragma once
#include <memory>

#include "Command.h"

// Forward declarations
class Plant;
class Inventory;

/**
 * @class AddToStorageCommand
 * @brief A concrete Command for moving a mature plant to the Storage group.
 *
 * This command is created by the NurserySupervisor when it detects a plant
 * has reached the Mature state. It moves the plant from its current owner
 * (typically a plot/growing area) to the "Storage" group where it can be sold.
 *
 * Preconditions:
 * - Plant must be in Mature state
 * - Plant must have a valid owner (current group)
 * - Inventory must have a "Storage" group
 *
 * If any precondition fails, execute() will set status to FAILED and return.
 */
class AddToStorageCommand : public Command {
   private:
    std::weak_ptr<Plant> targetPlant;
    std::weak_ptr<Inventory> inventory;
    Status currentStatus;
    uint64_t targetId = 0;

   public:
    /**
     * @brief Constructs a command to move a mature plant to storage
     * @param plant The plant to move (must be Mature)
     * @param inv The inventory containing the Storage group
     */
    AddToStorageCommand(const std::shared_ptr<Plant>& plant, const std::shared_ptr<Inventory>& inv);
    ~AddToStorageCommand() override = default;

    void execute() override;

    Status getStatus() const override;
    void setStatus(Status s) override;
    uint64_t getTargetId() const override;
    void setTargetId(uint64_t id) override;
    std::string toString() const override;
};