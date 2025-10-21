#pragma once

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
    virtual ~Iterator() {}
    virtual InventoryComponent* next() = 0;
    virtual bool hasNext() = 0;
};
