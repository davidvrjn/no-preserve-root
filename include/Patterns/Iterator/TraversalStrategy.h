#pragma once
#include <vector>

// Forward declaration
class InventoryComponent;

/**
 * @interface TraversalStrategy
 * @brief The interface for the Strategy design pattern, used by the CompositeIterator.
 * 
 * This interface defines a contract for different traversal algorithms (like
 * Pre-Order or Level-Order). This allows the iterator's traversal logic to be
 * changed at runtime.
 */
class TraversalStrategy {
public:
    virtual ~TraversalStrategy() {}
    virtual void traverse(InventoryComponent* component, std::vector<InventoryComponent*>& collection) = 0;
};
