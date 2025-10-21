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
    // Returns the next InventoryComponent as a shared_ptr (may return nullptr if none)
    virtual std::shared_ptr<InventoryComponent> next() = 0;
    // Reports whether a subsequent call to next() will produce a non-null result.
    virtual bool hasNext() const = 0;
};
