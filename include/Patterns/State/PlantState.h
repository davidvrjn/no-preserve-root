
#pragma once
#include <memory>

// Forward declaration to break circular dependency.
class Plant;

/**
 * @class PlantState
 * @brief The interface for the State design pattern.
 * 
 * This interface defines the contract that all concrete state classes must follow.
 * It declares the methods that the Plant (the "Context") will delegate to its
 * current state object. This allows the Plant's behavior to change dynamically
 * as its state object is swapped out.
 */
class PlantState {
public:
    virtual ~PlantState() = default;

    /**
     * @brief Handles the logic for transitioning out of the current state.
     * @param plant A pointer to the context (the Plant object).
     */
    virtual void handleStateChange(Plant* plant) = 0;

    /**
     * @brief Contains the core logic that is executed each day the Plant is in this state.
     * @param plant A pointer to the context (the Plant object).
     */
    virtual void performDailyActivity(Plant* plant) = 0;

    /**
     * @brief (Prototype Pattern) Allows state objects to be cloned.
     * 
     * This is necessary for the Plant's clone method to perform a deep copy.
     * When a Plant is cloned, its current state must also be cloned.
     * @return A unique_ptr to a new PlantState instance.
     */
    virtual std::unique_ptr<PlantState> clone() const = 0;
};


