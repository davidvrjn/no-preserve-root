#pragma once
#include <memory>
#include <string>
#include <vector>

#include "../Patterns/Observer/Subject.h"
#include "InventoryComponent.h"
#include "PlantAttributes.h"

// Forward declarations to break circular dependencies.
class PlantState;
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
    double price;  // Price in Rand (R)
    int age;
    int health;
    int waterLevel;
    int waterConsumption;
    int seedlingDuration;
    int growingDuration;

    // Plant characteristics for customer matching
    WaterRequirement waterRequirement;
    std::vector<Season> preferredSeasons;

    // PlantState ownership: each plant owns its state object
    std::unique_ptr<PlantState> currentState;  // (State Pattern) The current state of the plant.

    // Observers are stored as weak_ptrs to avoid ownership cycles and dangling pointers.
    std::vector<std::weak_ptr<Observer>> observers;

   public:
    Plant(const std::string& name, double price);
    ~Plant() override = default;

    // --- Overrides from InventoryComponent (Composite & Prototype) ---
    std::string getName() const override;
    double getPrice() const override;
    std::unique_ptr<Iterator> createIterator() override;
    std::shared_ptr<InventoryComponent> clone() const override;
    std::shared_ptr<InventoryComponent> blueprintClone() const override;
    std::string serialize() const override;
    void deserialize(const std::string& data) override;
    std::string typeName() const override;

    // --- Methods for State Pattern ---

    /**
     * @brief Sets the plant's current lifecycle state.
     * @param state A unique_ptr to the new state object (ownership transferred).
     */
    void setState(std::unique_ptr<PlantState> state);

    /**
     * @brief The main update method called each day, which delegates to the current state.
     */
    void performDailyActivity();

    // --- Methods for Observer Pattern ---

    /**
     * @brief Attaches an observer to this plant.
     * @param observer The observer to attach (shared ownership retained by caller).
     */
    void attach(const std::shared_ptr<Observer>& observer) override;

    /**
     * @brief Detaches an observer from this plant.
     * @param observer The observer to detach.
     */
    void detach(const std::shared_ptr<Observer>& observer) override;

    /**
     * @brief Notifies all attached observers of a state change.
     */
    void notify() override;

    // Detach all observers (called by owner before removing plant)
    void detachAllObservers() override;

    // --- Plant-specific methods ---

    /**
     * @brief The specific watering logic for this type of plant (polymorphic).
     */
    virtual void water() = 0;

    /**
     * @brief Fertilize the plant to restore health
     */
    virtual void fertilize();

    // Getters for state access
    int getAge() const { return age; }
    int getHealth() const { return health; }
    int getWaterLevel() const { return waterLevel; }
    int getWaterConsumption() const { return waterConsumption; }
    int getSeedlingDuration() const { return seedlingDuration; }
    int getGrowingDuration() const { return growingDuration; }

    // Getters for plant characteristics (for customer matching)
    WaterRequirement getWaterRequirement() const { return waterRequirement; }
    const std::vector<Season>& getPreferredSeasons() const { return preferredSeasons; }

    // Check if plant is suitable for a given season
    bool isSuitableForSeason(Season season) const;

    // Setters for state management (used by PlantState implementations)
    void setAge(int newAge) { age = newAge; }
    void setHealth(int newHealth) { health = newHealth; }
    void setWaterLevel(int newLevel) { waterLevel = newLevel; }
    void setWaterConsumption(int consumption) { waterConsumption = consumption; }
    void setSeedlingDuration(int duration) { seedlingDuration = duration; }
    void setGrowingDuration(int duration) { growingDuration = duration; }

   protected:
    // Helper for subclasses to set their characteristics
    void setCharacteristics(WaterRequirement water, const std::vector<Season>& seasons) {
        waterRequirement = water;
        preferredSeasons = seasons;
    }
};
