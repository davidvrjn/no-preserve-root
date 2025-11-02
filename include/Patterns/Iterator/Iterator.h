#pragma once
#include <memory>

// Forward declaration
class InventoryComponent;

/**
 * @interface Iterator
 * @brief The Iterator interface for the Iterator design pattern.
 *
 * Defines a standard way to traverse a collection of objects (our inventory)
 * without exposing its underlying representation (a tree structure).
 */
class Iterator {
   public:
    virtual ~Iterator() = default;
    /**
        @brief Retrieves the next inventory component is the sequence
        @return A shared poniter to the next InventoryComponent, or nullptr if none remian
    */
    virtual std::shared_ptr<InventoryComponent> next() = 0;
      /**
        @brief Checks whether there are more components to iterate over
        @return True if another component is available; flase otherwise
    */
    virtual bool hasNext() const = 0;
};
