
#pragma once
#include <functional>
#include <memory>
#include <vector>

#include "../Components/InventoryComponent.h"

// Forward declarations
class Plant;
class Group;
class Iterator;
class Observer;

/**
 * @class Inventory
 * @brief Manages the collection of all InventoryComponents in the nursery.
 *
 * This class is the top-level container for our Composite structure. It holds
 * the root-level plants and groups.
 *
 * NOTE: When adding newly created plants, the add() method only manages the composite structure.
 *       Business logic (seed cost deduction, state initialization) must be handled by the caller
 *       before calling add().
 */
class Inventory : public std::enable_shared_from_this<Inventory> {
   private:
    // Inventory owns its top-level components (shared ownership for flexibility).
    std::vector<std::shared_ptr<InventoryComponent>> components;

    // Callback invoked when a new plant is added (for attaching observers like NurserySupervisor)
    std::function<void(const std::shared_ptr<Plant>&)> onPlantAdded;

   public:
    Inventory();
    ~Inventory();

    void add(const std::shared_ptr<InventoryComponent>& component);
    void remove(const std::shared_ptr<InventoryComponent>& component);
    std::unique_ptr<Iterator>
    createIterator();  // Will create a CompositeIterator for the whole inventory.
    std::shared_ptr<Group> findGroupByName(const std::string& name);

    /**
     * @brief Set a callback to be invoked when plants are added
     * @param callback Function that receives newly added plants
     */
    void setOnPlantAddedCallback(std::function<void(const std::shared_ptr<Plant>&)> callback);

    // Serialization for Memento pattern
    std::string serialize() const;
    void deserialize(const std::string& data);

    // --- Query functions for clients ---

    /**
     * @brief Count total number of components (including nested)
     * @return Total component count
     */
    int countAllComponents() const;

    /**
     * @brief Count components by type name
     * @param typeName Type to count (e.g., "Rose", "Cactus", "Group")
     * @return Number of matching components
     */
    int countByType(const std::string& typeName) const;

    /**
     * @brief Find all components matching a predicate
     * @param predicate Function that returns true for matching components
     * @return Vector of matching components
     */
    std::vector<std::shared_ptr<InventoryComponent>> findAll(
        std::function<bool(const std::shared_ptr<InventoryComponent>&)> predicate) const;

    /**
     * @brief Get all plants (excludes groups)
     * @return Vector of all plant components
     */
    std::vector<std::shared_ptr<Plant>> getAllPlants() const;

    /**
     * @brief Get all groups
     * @return Vector of all group components
     */
    std::vector<std::shared_ptr<Group>> getAllGroups() const;
};
