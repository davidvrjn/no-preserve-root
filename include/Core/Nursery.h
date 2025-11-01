
#pragma once

#include <map>
#include <memory>
#include <queue>
#include <string>
#include <vector>

// Include necessary component and pattern interfaces.
// Use forward declarations where possible to reduce compilation dependencies.
class Inventory;
class Staff;
class Customer;
class NurserySupervisor;
class PlantFactory;
class PlantSpecificationBuilder;
class Command;
class Memento;
enum class Season;

/**
 * @enum GamePhase
 * @brief Represents the current phase of the simulation day.
 * 
 * The simulation day has 6 input frames:
 * - DAY_START: Before steps begin (plant seeds, view inventory)
 * - STEP_BREAK: Between steps (plant seeds, view inventory)
 * - DAY_END: After all steps (SAVE, HIRE, plant seeds, view inventory)
 */
enum class GamePhase {
    IDLE,        // No day in progress
    DAY_START,   // Day started, before steps (Frame 0)
    STEP_BREAK,  // Between steps (Frames 1-4)
    DAY_END      // All steps complete (Frame 5) - save/hire allowed
};

/**
 * @class Nursery
 * @brief The central coordinating class for the entire simulation.
 *
 * The Nursery class is the heart of the application. It owns and manages all major
 * subsystems and acts as the main entry point for the simulation logic. It is
 * responsible for:
 * - Managing the main game loop (`runSimulation`).
 * - Owning the `Inventory` and the list of `Staff`.
 * - Owning the `NurserySupervisor` to monitor plants.
 * - Holding the central `RequestQueue` for commands.
 * - Using factories to create new plants and builders to create customer requests.
 * - Acting as the "Originator" for the Memento pattern to save/load state.
 */
class Nursery : public std::enable_shared_from_this<Nursery> {
   private:
    int currentDay;
    int currentStep;        // Current step (0-4 during execution, 5 when complete)
    GamePhase currentPhase; // Current phase of the day

    // Business Metrics
    double money;    // Current cash balance
    int reputation;  // Reputation score (0-100)

    // Owned Subsystems
    std::shared_ptr<Inventory> inventory;
    // Head of the Chain of Responsibility
    std::shared_ptr<Staff> staffChainHead;
    std::shared_ptr<NurserySupervisor> supervisor;
    // Command logging for UI (tracks pending/completed commands per step)
    std::shared_ptr<class CommandLog> commandLog;

    // Data Structures
    // Nursery owns commands placed into its queue.
    std::queue<std::unique_ptr<Command>> requestQueue;
    std::map<std::string, std::shared_ptr<PlantFactory>> plantFactories;

    // Track plant types that have been grown (for customer PURCHASE requests)
    std::vector<std::string> knownPlantTypes;
    // Per-step bookkeeping for UI
    int customersSpawnedThisStep = 0;
    int customersLeftThisStep = 0;
    std::vector<std::string> completedCommandsThisStep;
    std::vector<std::string> remainingCommandsAtStepEnd;

        /**
     * @brief Contains the logic for dynamically spawning a new customer.
     *
     * This method uses the Builder pattern to construct a new customer request
     * based on the current state of the inventory.
     */
    void spawnCustomer();

    /**
     * @brief Initializes the nursery's starting state.
     *
     * Called by the constructor to set up the initial staff, inventory,
     * and factories.
     */
    void setupNursery();

   public:
    Nursery();
    ~Nursery();

    // --- Simulation Control ---

    /**
     * @brief Auto-runs all 5 steps of a day, then pauses at DAY_END phase.
     * 
     * "Speed through day" mode: Automatically executes startNewDay() and all
     * advanceStep() calls, then stops at DAY_END phase for user to save/hire.
     * 
     * After calling this, the nursery will be in DAY_END phase. User can save,
     * hire staff, etc. Call startNewDay() again to begin the next day.
     */
    void runSimulation();

    /**
     * @brief Starts a new simulation day.
     * 
     * Can be called from IDLE (first day) or DAY_END (after previous day completed).
     * Clears any remaining commands from previous day (staff clocked out), advances
     * currentDay, performs plant daily updates, and transitions to DAY_START phase.
     * 
     * User can plant seeds, view inventory, etc. Call advanceStep() when ready to
     * begin step execution.
     * 
     * @throws std::runtime_error if called during step execution (DAY_START or STEP_BREAK)
     */
    void startNewDay();

    /**
     * @brief Advances to the next step and executes it.
     * 
     * Executes one step (spawn customers, process commands, timeouts).
     * After execution, transitions to STEP_BREAK (if more steps remain) or
     * DAY_END (if all 5 steps complete).
     * 
     * @return true if step executed, false if day is already complete
     * @throws std::runtime_error if no day in progress (call startNewDay() first)
     */
    bool advanceStep();

    // --- Phase Queries ---

    /**
     * @brief Gets the current game phase.
     * @return Current GamePhase (IDLE, DAY_START, STEP_BREAK, or DAY_END)
     */
    GamePhase getCurrentPhase() const { return currentPhase; }

    /**
     * @brief Gets the current step number (0-5).
     * @return Step number (0-4 during execution, 5 when all complete)
     */
    int getCurrentStep() const { return currentStep; }

    /**
     * @brief Checks if all 5 steps are complete.
     * @return true if currentStep == 5 (day complete)
     */
    bool isDayComplete() const { return currentStep == 5; }

    /**
     * @brief Checks if save operation is allowed in current phase.
     * @return true only if currentPhase == DAY_END
     */
    bool canSave() const { return currentPhase == GamePhase::DAY_END; }

    /**
     * @brief Checks if hiring staff is allowed in current phase.
     * @return true only if currentPhase == DAY_END
     */
    bool canHire() const { return currentPhase == GamePhase::DAY_END; }

    /**
     * @brief Adds a command to the central request queue.
     *
     * This is called by components like the NurserySupervisor to queue up new tasks.
     * @param cmd The command to be added (ownership transferred).
     */
    void addRequest(std::unique_ptr<Command> cmd);

    // --- Per-step accessors for UI ---
    const std::vector<std::string>& getCompletedCommandsThisStep() const { return completedCommandsThisStep; }
    const std::vector<std::string>& getRemainingCommandsAtStepEnd() const { return remainingCommandsAtStepEnd; }
    int getCustomersSpawnedThisStep() const { return customersSpawnedThisStep; }
    int getCustomersLeftThisStep() const { return customersLeftThisStep; }

    // --- Memento Pattern (Originator Methods) ---

    /**
     * @brief Creates a Memento containing a snapshot of the nursery's current state.
     * @return A pointer to a new Memento object.
     */
    Memento* createMemento() const;

    /**
     * @brief Restores the nursery's state from a given Memento.
     * @param memento The Memento object to restore from.
     */
    void restoreFromMemento(Memento* memento);

    /**
     * @brief Gets the current season based on the current day.
     * @return The current Season (cycles through SPRING, SUMMER, FALL, WINTER every 30 days)
     */
    Season getCurrentSeason() const;

    /**
     * @brief Adds a plant type to the list of known plants.
     * @param plantType The name of the plant type to add (e.g., "Rose", "Cactus")
     *
     * Used to track which plant types have been successfully grown.
     * Customer PURCHASE requests will choose from this list.
     */
    void addKnownPlantType(const std::string& plantType);

    // --- Business Metrics ---

    /**
     * @brief Adds or subtracts money from the nursery's balance.
     * @param amount The amount to add (positive) or subtract (negative)
     *               Use negative values for expenses (seeds, fertilizer)
     */
    void adjustMoney(double amount);

    /**
     * @brief Adjusts the nursery's reputation.
     * @param change The amount to change reputation by (positive or negative)
     *               Reputation is clamped between 0 and 100
     */
    void adjustReputation(int change);

    /**
     * @brief Gets the current money balance.
     * @return Current money amount
     */
    double getMoney() const { return money; }

    /**
     * @brief Gets the current reputation score.
     * @return Reputation value (0-100)
     */
    int getReputation() const { return reputation; }

    /**
     * @brief Gets the current day number.
     * @return Current simulation day
     */
    int getCurrentDay() const { return currentDay; }

    /**
     * @brief Gets the nursery's inventory.
     * @return Shared pointer to the Inventory
     */
    std::shared_ptr<Inventory> getInventory() const { return inventory; }

    /**
     * @brief Gets the list of known plant types.
     * @return Vector of plant type names that have been grown
     */
    const std::vector<std::string>& getKnownPlantTypes() const { return knownPlantTypes; }

    /**
     * @brief Sets the head of the staff chain.
     * @param head The staff member to set as chain head
     */
    void setStaffChainHead(const std::shared_ptr<Staff>& head) { staffChainHead = head; }

    /**
     * @brief Gets the head of the staff chain.
     * @return Shared pointer to the staff chain head
     */
    std::shared_ptr<Staff> getStaffChainHead() const { return staffChainHead; }

    /**
     * @brief Gets the available plant factories.
     * @return Reference to the map of plant type name -> factory
     */
    const std::map<std::string, std::shared_ptr<PlantFactory>>& getPlantFactories() const { return plantFactories; }

    // --- Setup / Restore helpers ---
    /** Register built-in plant factories (called by setupNursery()) */
    void registerDefaultFactories();

    /** Attach the supervisor observer to all existing plants */
    void attachSupervisorToAllExistingPlants();

    /** Reinitialize runtime-only objects after loading a memento */
    void postRestoreInit();

    /** Safely create a Nursery inside a shared_ptr and run setupNursery() */
    static std::shared_ptr<Nursery> createAndSetup();

};
