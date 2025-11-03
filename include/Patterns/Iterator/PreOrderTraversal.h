#pragma once
#include <memory>

#include "TraversalStrategy.h"

// Forward declaration
class InventoryComponent;

/**
 * @class PreOrderTraversal
 * @brief A TraversalStrategy that visits the root first, then children in order.
 */
class PreOrderTraversal : public TraversalStrategy {
   public:
    PreOrderTraversal() = default;
    ~PreOrderTraversal() override = default;

    /**
     * @brief Traverses the component hierarchy and collects components.
     * 
     * Builds a flattened collection from the given root. Method is const
     * to ensure strategies remain stateless.
     * 
     * @param component The root component to start traversal from.
     * @param collection Output vector populated with components in traversal order.
     */
    void traverse(const std::shared_ptr<InventoryComponent>& component,
                  std::vector<std::shared_ptr<InventoryComponent>>& collection) const override;
};
