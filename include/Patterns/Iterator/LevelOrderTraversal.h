#pragma once
#include "TraversalStrategy.h"
#include <memory>

// Forward declaration
class InventoryComponent;

/**
 * @class LevelOrderTraversal
 * @brief A TraversalStrategy that visits nodes level by level (breadth-first).
 */
class LevelOrderTraversal : public TraversalStrategy {
public:
	LevelOrderTraversal() = default;
	~LevelOrderTraversal() override = default;

	void traverse(const std::shared_ptr<InventoryComponent>& component,
				  std::vector<std::shared_ptr<InventoryComponent>>& collection) const override;
};
