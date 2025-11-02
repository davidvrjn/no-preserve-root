#include "../../include/Actors/Cashier.h"

#include <memory>

#include "../../include/Patterns/Command/Command.h"
#include "../../include/Patterns/Command/FulfillCustomerCommand.h"

/**
 * @brief Constructs a Cashier staff member
 * 
 * Initializes a cashier with default Staff properties (not busy, no successor).
 * Cashiers are responsible for handling customer transactions and fulfillment requests.
 */
Cashier::Cashier() : Staff() {}

/**
 * @brief Handles incoming commands using Chain of Responsibility pattern
 * 
 * @param cmd Unique pointer to the command to handle (ownership transferred)
 * 
 * Processing logic:
 * 1. If command is null, return immediately
 * 2. Attempt to cast to FulfillCustomerCommand
 * 3. If cast succeeds and cashier is not busy:
 *    - Mark as busy
 *    - Execute the command
 *    - Mark as not busy
 * 4. If cast succeeds but cashier is busy, forward to successor
 * 5. If cast fails (not a customer command), forward to successor
 * 6. If no successor exists, command is dropped
 * 
 * This implements the Chain of Responsibility pattern where each staff member
 * handles specific command types and forwards others down the chain.
 */
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