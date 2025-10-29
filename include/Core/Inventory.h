
#pragma once
#include <memory>
#include <vector>

#include "../Components/InventoryComponent.h"

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

   public:
    Inventory();
    ~Inventory();

    void add(const std::shared_ptr<InventoryComponent>& component);
    void remove(const std::shared_ptr<InventoryComponent>& component);
    std::unique_ptr<Iterator>
    createIterator();  // Will create a CompositeIterator for the whole inventory.
    std::shared_ptr<Group> findGroupByName(const std::string& name);

    // Serialization for Memento pattern
    std::string serialize() const;
    void deserialize(const std::string& data);
};
