
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
class NurserySupervisor;
class PlantFactory;
class PlantSpecificationBuilder;
class Command;
class Memento;
enum class Season;

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

    // Business Metrics
    double money;    // Current cash balance
    int reputation;  // Reputation score (0-100)

    // Owned Subsystems
    std::shared_ptr<Inventory> inventory;
    // Head of the Chain of Responsibility
    std::shared_ptr<Staff> staffChainHead;
    std::shared_ptr<NurserySupervisor> supervisor;

    // Data Structures
    // Nursery owns commands placed into its queue.
    std::queue<std::unique_ptr<Command>> requestQueue;
    std::map<std::string, std::shared_ptr<PlantFactory>> plantFactories;

    // Track plant types that have been grown (for customer PURCHASE requests)
    std::vector<std::string> knownPlantTypes;

   public:
    Nursery();
    ~Nursery();

    /**
     * @brief The main game loop. This method drives the entire simulation.
     */
    void runSimulation();

    /**
     * @brief Adds a command to the central request queue.
     *
     * This is called by components like the NurserySupervisor to queue up new tasks.
     * @param cmd The command to be added (ownership transferred).
     */
    void addRequest(std::unique_ptr<Command> cmd);

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

   private:
    // --- Private Helper Methods for the Game Loop ---

    /**
     * @brief Contains the logic for dynamically spawning a new customer.
     *
     * This method uses the Builder pattern to construct a new customer request
     * based on the current state of the inventory.
     */
    void spawnCustomer();

    /**
     * @brief Processes all commands currently in the request queue.
     *
     * This method dequeues commands and passes them to the head of the
     * Staff's Chain of Responsibility.
     */
    void processRequestQueue();

    /**
     * @brief Initializes the nursery's starting state.
     *
     * Called by the constructor to set up the initial staff, inventory,
     * and factories.
     */
    void setupNursery();
};
