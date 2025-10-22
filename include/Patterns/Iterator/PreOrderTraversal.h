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

    void traverse(const std::shared_ptr<InventoryComponent>& component,
                  std::vector<std::shared_ptr<InventoryComponent>>& collection) const override;
};
