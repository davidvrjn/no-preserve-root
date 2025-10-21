#pragma once
#include "Command.h"

// Forward declaration
class Plant;

/**
 * @class WaterPlantCommand
 * @brief A concrete Command for watering a specific plant.
 * 
 * This object holds all the context needed to perform its action, namely a
 * reference to the Plant that needs to be watered. Its execute() method
 * contains the final, specific action of calling the plant's water() method.
 */
class WaterPlantCommand : public Command {
private:
	Plant& targetPlant; // A reference, not a pointer, to ensure it's not null.

public:
	WaterPlantCommand(Plant& plant);
	~WaterPlantCommand();

	void execute() override;
};

