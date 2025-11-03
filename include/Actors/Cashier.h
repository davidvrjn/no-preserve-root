
#pragma once
#include <memory>

#include "Staff.h"

/**
 * @class Cashier
 * @brief A concrete handler in the Chain of Responsibility for customer-related commands.
 */
class Cashier : public Staff {
   public:
    Cashier();
    ~Cashier() override = default;

    void handleRequest(std::unique_ptr<Command> cmd) override;
};
