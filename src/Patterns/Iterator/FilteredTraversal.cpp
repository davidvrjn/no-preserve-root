
#include "Patterns/Iterator/FilteredTraversal.h"

FilteredTraversal::FilteredTraversal(
    std::unique_ptr<TraversalStrategy> strategy,
    std::function<bool(const std::shared_ptr<InventoryComponent>&)> filter)
    : baseStrategy(std::move(strategy)), predicate(filter) {}

void FilteredTraversal::traverse(
    const std::shared_ptr<InventoryComponent>& component,
    std::vector<std::shared_ptr<InventoryComponent>>& collection) const {
    
    if (!component) {
        return;
    }
    
    // Perform the full traversal using the base strategy
    std::vector<std::shared_ptr<InventoryComponent>> allComponents;
    baseStrategy->traverse(component, allComponents);
    
    // Filter the results based on the predicate
    for (const auto& comp : allComponents) {
        if (predicate(comp)) {
            collection.push_back(comp);
        }
    }
}
