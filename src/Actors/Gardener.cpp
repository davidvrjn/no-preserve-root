#include "../../include/Actors/Gardener.h"

#include <memory>

#include "../../include/Patterns/Command/Command.h"
#include "../../include/Patterns/Command/LoggingCommand.h"
#include "../../include/Patterns/Command/WaterPlantCommand.h"

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

    // Try to cast to WaterPlantCommand (Gardener handles plant care)
    auto waterCmd = dynamic_cast<WaterPlantCommand*>(actualCmd);

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
