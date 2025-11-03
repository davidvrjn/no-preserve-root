
#pragma once
#include <memory>
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
    virtual ~TraversalStrategy() = default;
    /**
     * @brief Traverses the component hierarchy in pre-order fashion.
     * @param component The root component to start traversal from.
     * @param collection Output vector populated with components in pre-order.
     */
    virtual void traverse(const std::shared_ptr<InventoryComponent>& component,
                          std::vector<std::shared_ptr<InventoryComponent>>& collection) const = 0;
};
