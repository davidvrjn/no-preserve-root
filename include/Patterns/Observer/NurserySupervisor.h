#pragma once
#include "Observer.h"

// Forward declaration
class Nursery;

/**
 * @class NurserySupervisor
 * @brief A concrete Observer that monitors plants for changes.
 * 
 * This class's sole responsibility is to translate a notification from a Plant
 * into an actionable Command. It observes a Plant, and when its update() method
 * is called, it inspects the Plant's state and creates the appropriate Command
 * (e.g., WaterPlantCommand), adding it to the Nursery's central request queue.
 */
class NurserySupervisor : public Observer {
private:
	Nursery* nursery; // A reference back to the main nursery to access the queue.

public:
	NurserySupervisor(Nursery* nursery);
	~NurserySupervisor();

	void update(Subject* subject) override;
};

