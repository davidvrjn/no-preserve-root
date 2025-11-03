#pragma once
#include "PlantState.h"
#include "Withering.h"

/**
 * @class Mature
 * @brief Represents the mature state of a plant in its lifecycle.
 * 
 * This state represents a fully grown, healthy plant that has reached its
 * peak development. A mature plant can transition to the withering state
 * if neglected or conditions deteriorate.
 * Follows the State pattern to manage plant lifecycle behavior.
 */
class Mature : public PlantState {
   public:
   /**
    * @brief Construct a new Mature object
    * 
    */
    Mature();
    /**
     * @brief Destroy the Mature object
     * 
     */
    ~Mature() override = default;
    /**
     * @brief Handles state transitions for a plant in the mature state.
     * 
     * Evaluates conditions and may transition the plant to withering
     * state if care conditions deteriorate.
     * 
     * @param plant Pointer to the plant whose state is being managed.
     */
    void handleStateChange(Plant* plant) override;
    /**
     * @brief Performs the daily activities for a mature plant.
     * 
     * Executes maintenance behaviors and updates plant attributes.
     * 
     * @param plant Pointer to the plant performing daily activities.
     */
    void performDailyActivity(Plant* plant) override;
    /**
     * @brief Creates a clone of this mature state.
     * @return A unique pointer to a cloned Mature state object.
     */
    std::unique_ptr<PlantState> clone() const override;
};
