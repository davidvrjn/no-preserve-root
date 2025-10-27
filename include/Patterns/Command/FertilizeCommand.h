
#pragma once
#include <memory>

#include "Command.h"

// Forward declarations
class Plant;
class Nursery;

/**
 * @class FertilizeCommand
 * @brief A concrete Command for fertilizing a withering plant.
 *
 * This command attempts to restore a withering plant by applying fertilizer,
 * which costs R30. The fertilizer should transition the plant from the
 * Withering state back to its previous state.
 *
 * The command holds:
 * - A weak reference to the target Plant
 * - A weak reference to the Nursery (to deduct fertilizer cost)
 *
 * @note Implementation stub - full logic to be completed by team
 */
class FertilizeCommand : public Command {
   private:
    std::weak_ptr<Plant> targetPlant;   // Non-owning reference; may be expired.
    std::weak_ptr<Nursery> nursery;     // Non-owning reference to Nursery
    Status currentStatus;
    uint64_t targetId = 0;

    static constexpr double fertCost = 30.0;  // R30 per fertilization

   public:
    /**
     * @brief Construct a FertilizeCommand for a specific plant
     * @param plant The plant to fertilize
     * @param nur The nursery (for cost deduction)
     */
    FertilizeCommand(const std::shared_ptr<Plant>& plant, const std::shared_ptr<Nursery>& nur);
    ~FertilizeCommand() override = default;
    void execute() override;

    std::string serialize() const override;
    void deserialize(const std::string& data) override;
    Status getStatus() const override;
    void setStatus(Status s) override;
    uint64_t getTargetId() const override;
    void setTargetId(uint64_t id) override;
};
