#pragma once
#include "InventoryComponent.h"
#include "../Patterns/Observer/Subject.h"
#include <string>
#include <vector>

// Forward declarations to break circular dependencies.
class PlantState;
class Subject;
class Observer;

/**
 * @class Plant
 * @brief Represents a single plant in the nursery (a "Leaf" in the Composite pattern).
 *
 * This class is a concrete implementation of InventoryComponent. It holds all the
 * intrinsic properties of a plant, such as health, water level, and age.
 *
 * It plays multiple roles in other patterns:
 * - It is the "Context" for the State pattern, delegating its behavior to a PlantState object.
 * - It is the "Subject" for the Observer pattern, notifying observers of state changes.
 */
class Plant : public InventoryComponent, public Subject {
private:
	std::string name;
	double price;
	int age;
	int health;
	int waterLevel;
	PlantState* currentState; // (State Pattern) The current state of the plant.
	std::vector<Observer*> observers; // (Observer Pattern) List of observers.

public:
	Plant(const std::string& name, double price);
	virtual ~Plant();

	// --- Overrides from InventoryComponent (Composite & Prototype) ---
	std::string getName() const override;
	double getPrice() const override;
	Iterator* createIterator() override;
	InventoryComponent* clone() const override;

	// --- Methods for State Pattern ---
    
	/**
	 * @brief Sets the plant's current lifecycle state.
	 * @param state A pointer to the new state object.
	 */
	void setState(PlantState* state);

	/**
	 * @brief The main update method called each day, which delegates to the current state.
	 */
	void performDailyActivity();

	// --- Methods for Observer Pattern ---

	/**
	 * @brief Attaches an observer to this plant.
	 * @param observer The observer to attach.
	 */
	void attach(Observer* observer) override;

	/**
	 * @brief Detaches an observer from this plant.
	 * @param observer The observer to detach.
	 */
	void detach(Observer* observer) override;
    
	/**
	 * @brief Notifies all attached observers of a state change.
	 */
	void notify() override;

	// --- Plant-specific methods ---

	/**
	 * @brief The specific watering logic for this type of plant (polymorphic).
	 */
	virtual void water() = 0;
};

