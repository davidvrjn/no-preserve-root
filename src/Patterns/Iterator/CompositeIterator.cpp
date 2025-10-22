#include "../../../include/Patterns/Iterator/CompositeIterator.h"

#include "../../../include/Patterns/Iterator/TraversalStrategy.h"

CompositeIterator::CompositeIterator(const std::shared_ptr<InventoryComponent>& root,
                                     std::unique_ptr<TraversalStrategy> traversalStrategy)
    : strategy(std::move(traversalStrategy)) {
    (void)root;
}

CompositeIterator::~CompositeIterator() = default;

std::shared_ptr<InventoryComponent> CompositeIterator::next() { return nullptr; }

bool CompositeIterator::hasNext() const { return false; }
