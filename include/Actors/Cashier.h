#pragma once
#include "Staff.h"

/**
 * @class Cashier
 * @brief A concrete handler in the Chain of Responsibility for customer-related commands.
 */
class Cashier : public Staff {
public:
	Cashier();
	~Cashier();

	void handleRequest(Command* cmd) override;
};

