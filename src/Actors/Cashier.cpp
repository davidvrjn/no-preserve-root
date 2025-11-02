#include "../../include/Actors/Cashier.h"

#include <memory>

#include "../../include/Patterns/Command/Command.h"
#include "../../include/Patterns/Command/FulfillCustomerCommand.h"

Cashier::Cashier() : Staff() {}

void Cashier::handleRequest(std::unique_ptr<Command> cmd) {
    if (!cmd) {
        return;
    }

    // Try to cast to FulfillCustomerCommand (Cashier handles customer requests)
    auto customerCmd = dynamic_cast<FulfillCustomerCommand*>(cmd.get());

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
        setBusy(false);

        return;
    }

    // Not a command this handler can process, forward to successor
    if (successor) {
        successor->handleRequest(std::move(cmd));
    }

    // If no successor and can't handle, command is dropped
}