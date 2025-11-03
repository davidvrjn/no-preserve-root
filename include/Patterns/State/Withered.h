#pragma once
#include "PlantState.h"

/**
 * @class Withered
 * @brief Represents the final withered state of a plant in its lifecycle.
 *
 * This is a terminal state representing a plant that has completely died.
 * Once a plant reaches this state, it cannot transition to any other state.
 * This state typically results from prolonged neglect or severe conditions.
 * Follows the State pattern to manage plant lifecycle behavior.
 */
class Withered : public PlantState {
   public:
    /**
     * @brief Construct a new Withered object
     *
     */
    Withered();
    /**
     * @brief Destroy the Withered object
     *
     */
    ~Withered() override = default;
    /**
     * @brief Handles state transitions for a plant in the withered state.
     *
     * As this is a terminal state, no transitions occur. The method handles
     * any final state management requirements.
     *
     * @param plant Pointer to the plant whose state is being managed.
     */
    void handleStateChange(Plant* plant) override;
    /**
     * @brief Performs the daily activities for a withered plant.
     *
     * Minimal or no activity occurs as the plant is dead.
     *
     * @param plant Pointer to the plant performing daily activities.
     */
    void performDailyActivity(Plant* plant) override;
    /**
     * @brief Creates a clone of this withered state.
     * @return A unique pointer to a cloned Withered state object.
     */
    std::unique_ptr<PlantState> clone() const override;
};
