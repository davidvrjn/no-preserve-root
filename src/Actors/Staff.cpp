#include "../../include/Actors/Staff.h"

#include "../../include/Patterns/Command/Command.h"

/**
 * @brief Constructs a Staff member with default properties
 * 
 * Initializes a staff member with:
 * - No successor in the Chain of Responsibility (nullptr)
 * - Not busy status (available to handle requests)
 * 
 * Derived classes (Cashier, Gardener) inherit these defaults and can be
 * chained together to form a command handling pipeline.
 */
Staff::Staff() : successor(nullptr), busy(false) {}

/**
 * @brief Sets the next handler in the Chain of Responsibility
 * 
 * @param next Shared pointer to the staff member who should receive forwarded commands
 * 
 * Establishes the chain by linking this staff member to the next handler.
 * When this staff member cannot handle a command (either wrong type or busy),
 * the command is forwarded to the successor.
 * 
 * @note noexcept guarantees this operation won't throw exceptions
 */
void Staff::setSuccessor(const std::shared_ptr<Staff>& next) noexcept { successor = next; }
