#include "../../../include/Patterns/Iterator/CompositeIterator.h"

#include "../../../include/Components/InventoryComponent.h"
#include "../../../include/Patterns/Iterator/TraversalStrategy.h"

/**
 * @brief Constructs a CompositeIterator using the given traversal strategy
 * 
 * The constructor uses the strategy to build a flattened collection of all
 * components in the tree, then initializes the iterator position.
 */
CompositeIterator::CompositeIterator(const std::shared_ptr<InventoryComponent>& root,
                                     std::unique_ptr<TraversalStrategy> traversalStrategy)
    : strategy(std::move(traversalStrategy)) {
    
    if (root && strategy) {
        // Use the strategy to traverse the tree and build the collection
        strategy->traverse(root, collection);
    }
    
    // Initialize iterator position to the beginning
    position = collection.begin();
}

CompositeIterator::~CompositeIterator() = default;

/**
 * @brief Returns the next component in the iteration
 * @return shared_ptr to the next component, or nullptr if no more elements
 */
std::shared_ptr<InventoryComponent> CompositeIterator::next() {
    if (hasNext()) {
        auto current = *position;
        ++position;
        return current;
    }
    return nullptr;
}

/**
 * @brief Checks if there are more elements to iterate
 * @return true if next() will return a non-null element
 */
bool CompositeIterator::hasNext() const {
    return position != collection.end();
}
