#include "../../include/Actors/Cashier.h"

#include <memory>

#include "../../include/Patterns/Command/Command.h"
#include "../../include/Patterns/Command/FulfillCustomerCommand.h"
#include "../../include/Patterns/Command/LoggingCommand.h"

Cashier::Cashier() : Staff() {}

void Cashier::handleRequest(std::unique_ptr<Command> cmd) {
    if (!cmd) {
        return;
    }

    // Unwrap LoggingCommand if present to check the actual command type
    Command* actualCmd = cmd.get();
    auto* loggingCmd = dynamic_cast<LoggingCommand*>(actualCmd);
    if (loggingCmd) {
        actualCmd = loggingCmd->getInnerCommand();
    }

    // Try to cast to FulfillCustomerCommand (Cashier handles customer requests)
    auto customerCmd = dynamic_cast<FulfillCustomerCommand*>(actualCmd);

    if (customerCmd != nullptr) {
        // This is a customer fulfillment command - Cashier can handle it
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
        // Don't set busy back to false - staff stays busy for the rest of the step
        // Staff are reset to not-busy at the start of the next step

        return;
    }

    // Not a command this handler can process, forward to successor
    if (successor) {
        successor->handleRequest(std::move(cmd));
    }

    // If no successor and can't handle, command is dropped
}