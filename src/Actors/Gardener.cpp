#include "../../include/Actors/Gardener.h"

#include <memory>

#include "../../include/Patterns/Command/Command.h"
#include "../../include/Patterns/Command/LoggingCommand.h"
#include "../../include/Patterns/Command/WaterPlantCommand.h"
#include "../../include/Patterns/Command/FertilizeCommand.h"
#include "../../include/Patterns/Command/RemoveWitheredPlantCommand.h"

Gardener::Gardener() : Staff() {}

void Gardener::handleRequest(std::unique_ptr<Command> cmd) {
    //(void)cmd;  // stub: real logic implemented later

    if (!cmd) {
        return;
    }

    // Unwrap LoggingCommand if present to check the actual command type
    Command* actualCmd = cmd.get();
    auto* loggingCmd = dynamic_cast<LoggingCommand*>(actualCmd);
    if (loggingCmd) {
        actualCmd = loggingCmd->getInnerCommand();
    }

    // Try to cast to commands the gardener can handle (Gardener handles plant care)
    //Consider implementing a method called canHandle that can be called universally
    auto waterCmd = dynamic_cast<WaterPlantCommand*>(actualCmd);
    auto fertilizeCmd = dynamic_cast<FertilizeCommand*>(actualCmd);
    auto removeCmd = dynamic_cast<RemoveWitheredPlantCommand*>(actualCmd);

    if (waterCmd != nullptr || fertilizeCmd != nullptr || removeCmd != nullptr) {
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
