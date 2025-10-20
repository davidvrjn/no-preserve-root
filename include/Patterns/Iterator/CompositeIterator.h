#pragma once
#include "Iterator.h"
#include <vector>

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
    std::vector<InventoryComponent*> collection;
    std::vector<InventoryComponent*>::iterator position;
    TraversalStrategy* strategy;

public:
    CompositeIterator(InventoryComponent* root, TraversalStrategy* traversalStrategy);
    ~CompositeIterator();

    InventoryComponent* next() override;
    bool hasNext() override;
};
