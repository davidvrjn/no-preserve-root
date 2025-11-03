#include "../../../include/Patterns/Iterator/CompositeIterator.h"

#include "../../../include/Components/InventoryComponent.h"
#include "../../../include/Patterns/Iterator/TraversalStrategy.h"

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

std::shared_ptr<InventoryComponent> CompositeIterator::next() {
    if (hasNext()) {
        auto current = *position;
        ++position;
        return current;
    }
    return nullptr;
}

bool CompositeIterator::hasNext() const { return position != collection.end(); }
