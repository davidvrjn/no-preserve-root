#pragma once
#include "PlantState.h"

// Transitions
#include "Mature.h"
#include "Withering.h"

/**
 * @class Growing
 * @brief Represents the growing state of a plant in its lifecycle.
 * 
 * This state occurs after the seedling stage and before the plant reaches maturity.
 * During the growing state, the plant continues to develop and can transition to
 * either the mature state or the withering state based on care conditions.
 * Follows the State pattern to manage plant lifecycle behavior.
 */
class Growing : public PlantState {
   public:
   /**
     * @brief Constructs a Growing state.
     */
    Growing();
    /**
     * @brief Default destructor.
     */
    ~Growing() override = default;
    /**
     * @brief Handles state transitions for a plant in the growing state.
     * 
     * Evaluates conditions and transitions the plant to mature or withering
     * state as appropriate.
     * 
     * @param plant Pointer to the plant whose state is being managed.
     */
    void handleStateChange(Plant* plant) override;
    /**
     * @brief Performs the daily activities for a growing plant.
     * 
     * Executes growth-specific behaviors and updates plant attributes.
     * 
     * @param plant Pointer to the plant performing daily activities.
     */
    void performDailyActivity(Plant* plant) override;
    /**
     * @brief Creates a clone of this growing state.
     * @return A unique pointer to a cloned Growing state object.
     */
    std::unique_ptr<PlantState> clone() const override;
};
