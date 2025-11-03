#pragma once
#include "PlantState.h"
#include "Withered.h"

/**
 * @class Withering
 * @brief Represents the withering state of a plant in its lifecycle.
 *
 * This is a transitional declining state where the plant is deteriorating
 * due to neglect or poor conditions. The plant stores its previous healthy state
 * and can potentially recover if conditions improve, or will transition to
 * the withered (dead) state if conditions continue to decline.
 * Follows the State pattern to manage plant lifecycle behavior.
 */
class Withering : public PlantState {
   public:
    /**
     * @brief Constructs a Withering state with the previous state stored.
     *
     * The previous state is preserved to allow potential recovery if
     * conditions improve.
     *
     * @param prev The previous state the plant was in before withering.
     */
    Withering(std::unique_ptr<PlantState> prev);
    /**
     * @brief Destroy the Withering object
     *
     */
    ~Withering() override = default;
    /**
     * @brief Handles state transitions for a plant in the withering state.
     *
     * Evaluates conditions and may transition the plant back to its previous
     * state if care improves, or to the withered state if conditions worsen.
     *
     * @param plant Pointer to the plant whose state is being managed.
     */
    void handleStateChange(Plant* plant) override;
    /**
     * @brief Performs the daily activities for a withering plant.
     *
     * Executes declining health behaviors and updates plant attributes.
     *
     * @param plant Pointer to the plant performing daily activities.
     */
    void performDailyActivity(Plant* plant) override;
    /**
     * @brief Creates a clone of this withering state.
     *
     * Clones both this state and the stored previous state.
     *
     * @return A unique pointer to a cloned Withering state object.
     */
    std::unique_ptr<PlantState> clone() const override;

    /**
     * @brief Gets the previous state before withering began.
     *
     * Helper method for serialization that provides access to the
     * stored previous state.
     *
     * @return A const pointer to the previous state, or nullptr if none exists.
     */
    const PlantState* getPreviousState() const { return previousState.get(); }

   private:
    /**
     * @brief The state the plant was in before beginning to wither.
     *
     * Stored to allow recovery if conditions improve.
     */
    std::unique_ptr<PlantState> previousState;
};
