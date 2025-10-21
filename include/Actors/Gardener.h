#pragma once
#include "Staff.h"

/**
 * @class Gardener
 * @brief A concrete handler in the Chain of Responsibility.
 * 
 * This staff member is responsible for handling plant-care related commands
 * like WaterPlantCommand.
 */
class Gardener : public Staff {
public:
	Gardener();
	~Gardener();

	void handleRequest(Command* cmd) override;
};

