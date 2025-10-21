
#pragma once
#include "Iterator.h"
#include <vector>
#include <memory>

// Forward declarations
class InventoryComponent;
class TraversalStrategy;

/**
 * @class CompositeIterator
 * @brief A concrete Iterator that can traverse a Composite tree structure.
 * 
 * This iterator uses a TraversalStrategy to build a flattened list of all nodes
 * in the tree, which it then iterates over. This design separates the act of
 * iteration from the algorithm used for traversal.
 */
class CompositeIterator : public Iterator {
private:
    std::vector<std::shared_ptr<InventoryComponent>> collection;
    // Use an index for stability if the collection is ever rebuilt; implementation may use either.
    std::vector<std::shared_ptr<InventoryComponent>>::iterator position;
    // Own the strategy to make ownership clear
    std::unique_ptr<TraversalStrategy> strategy;

public:
    // root is a shared_ptr to the root component to traverse; strategy is owned by the iterator
    CompositeIterator(const std::shared_ptr<InventoryComponent>& root, std::unique_ptr<TraversalStrategy> traversalStrategy);
    ~CompositeIterator();

    std::shared_ptr<InventoryComponent> next() override;
    bool hasNext() const override;
};
