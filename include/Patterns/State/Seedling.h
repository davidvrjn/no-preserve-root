#pragma once
#include "PlantState.h"

// Include states that can be transitioned to
#include "Growing.h"
#include "Withering.h"

/**
 * @class Seedling
 * @brief Represents the initial seedling state of a plant in its lifecycle.
 *
 * This is the starting state for a newly planted seed. During the seedling stage,
 * the plant is most vulnerable and requires careful attention. It can transition
 * to the growing state with proper care, or to the withering state if neglected.
 * Follows the State pattern to manage plant lifecycle behavior.
 */
class Seedling : public PlantState {
   public:
    /**
     * @brief Construct a new Seedling object
     *
     */
    Seedling();
    /**
     * @brief Destroy the Seedling object
     *
     */
    ~Seedling() override = default;
    /**
     * @brief Handles state transitions for a plant in the seedling state.
     *
     * Evaluates conditions and transitions the plant to growing or withering
     * state as appropriate based on care and environmental factors.
     *
     * @param plant Pointer to the plant whose state is being managed.
     */
    void handleStateChange(Plant* plant) override;
    /**
     * @brief Performs the daily activities for a seedling plant.
     *
     * Executes early-stage growth behaviors and updates plant attributes.
     *
     * @param plant Pointer to the plant performing daily activities.
     */
    void performDailyActivity(Plant* plant) override;
    /**
     * @brief Creates a clone of this seedling state.
     * @return A unique pointer to a cloned Seedling state object.
     */
    std::unique_ptr<PlantState> clone() const override;
};
