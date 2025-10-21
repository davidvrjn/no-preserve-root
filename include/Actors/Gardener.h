
#pragma once
#include "Staff.h"
#include <memory>

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
    ~Gardener() override = default;

    void handleRequest(std::unique_ptr<Command> cmd) override;
};

