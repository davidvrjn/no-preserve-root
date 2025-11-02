#include "../../include/Actors/Gardener.h"

#include <memory>

#include "../../include/Patterns/Command/Command.h"
#include "../../include/Patterns/Command/WaterPlantCommand.h"

/**
 * @brief Constructs a Gardener staff member
 * 
 * Initializes a gardener with default Staff properties (not busy, no successor).
 * Gardeners are specialized staff members who handle plant care requests
 * in the Chain of Responsibility pattern.
 */
Gardener::Gardener() : Staff() {}

/**
 * @brief Handles incoming commands using Chain of Responsibility pattern
 * 
 * @param cmd Unique pointer to the command to handle (ownership transferred)
 * 
 * Processing logic:
 * 1. If command is null, return immediately
 * 2. Attempt to cast to WaterPlantCommand
 * 3. If cast succeeds and gardener is not busy:
 *    - Mark as busy
 *    - Execute the command (water the plant)
 *    - Mark as not busy
 * 4. If cast succeeds but gardener is busy, forward to successor
 * 5. If cast fails (not a plant care command), forward to successor
 * 6. If no successor exists, command is dropped
 * 
 * This implements the Chain of Responsibility pattern where each staff member
 * handles specific command types (gardeners handle plant watering) and forwards
 * others down the chain to the next available handler.
 */
void Gardener::handleRequest(std::unique_ptr<Command> cmd) {
    //(void)cmd;  // stub: real logic implemented later

    if (!cmd) {
        return;
    }

    // Try to cast to WaterPlantCommand (Gardener handles plant care)
    auto waterCmd = dynamic_cast<WaterPlantCommand*>(cmd.get());

    if (waterCmd != nullptr) {
        // This is a water plant command - Gardener can handle it
        if (isBusy()) {
            // Busy, pass to successor
            if (successor) {
                successor->handleRequest(std::move(cmd));
            }

            // If no successor, command is dropped (could log this)
            return;
        }

        // Not busy, handle the command
        setBusy(true);
        cmd->execute();
        setBusy(false);

        return;
    }

    // Not a command this handler can process, forward to successor
    if (successor) {
        successor->handleRequest(std::move(cmd));
    }

    // If no successor and can't handle, command is dropped
}
