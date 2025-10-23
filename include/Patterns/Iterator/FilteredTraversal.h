
#pragma once
#include "TraversalStrategy.h"
#include <functional>
#include <memory>

/**
 * @class FilteredTraversal
 * @brief A traversal strategy decorator that filters components based on a predicate
 * 
 * This class wraps any existing TraversalStrategy and applies a filter to the results.
 * This allows for creating custom iterators that only return specific types of components,
 * such as plants from a specific season, plants above a certain price, etc.
 * 
 * @example
 * ```cpp
 * // Filter for only Plants (exclude Groups)
 * auto plantOnlyFilter = [](const std::shared_ptr<InventoryComponent>& comp) {
 *     return std::dynamic_pointer_cast<Plant>(comp) != nullptr;
 * };
 * 
 * auto filteredStrategy = std::make_unique<FilteredTraversal>(
 *     std::make_unique<PreOrderTraversal>(),
 *     plantOnlyFilter
 * );
 * 
 * auto iterator = group->createIterator(std::move(filteredStrategy));
 * ```
 * 
 * @see TraversalStrategy
 * @see PreOrderTraversal
 * @see LevelOrderTraversal
 */
class FilteredTraversal : public TraversalStrategy {
   private:
    std::unique_ptr<TraversalStrategy> baseStrategy;
    std::function<bool(const std::shared_ptr<InventoryComponent>&)> predicate;
    
   public:
    /**
     * @brief Constructs a FilteredTraversal with a base strategy and filter predicate
     * 
     * @param strategy The underlying traversal strategy (PreOrder, LevelOrder, etc.)
     * @param filter A predicate function that returns true for components to include
     */
    FilteredTraversal(std::unique_ptr<TraversalStrategy> strategy,
                      std::function<bool(const std::shared_ptr<InventoryComponent>&)> filter);
    
    /**
     * @brief Traverses the component tree and collects only components matching the filter
     * 
     * First performs the full traversal using the base strategy, then applies the
     * filter predicate to include only matching components in the collection.
     * 
     * @param component The root component to start traversal from
     * @param collection Output vector to store filtered components
     */
    void traverse(const std::shared_ptr<InventoryComponent>& component,
                  std::vector<std::shared_ptr<InventoryComponent>>& collection) const override;
};
