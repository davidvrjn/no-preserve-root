#include "../../../include/Patterns/Iterator/LevelOrderTraversal.h"

#include <queue>

#include "../../../include/Components/Group.h"
#include "../../../include/Components/InventoryComponent.h"

/**
 * @brief Level-order traversal: visits nodes level-by-level (breadth-first)
 * 
 * Algorithm:
 * 1. Use a queue to process nodes level by level
 * 2. Visit current node, then enqueue all its children
 * 3. Continue until queue is empty
 */
void LevelOrderTraversal::traverse(
    const std::shared_ptr<InventoryComponent>& component,
    std::vector<std::shared_ptr<InventoryComponent>>& collection) const {
    
    if (!component) {
        return;  // Null safety
    }

    // Queue for breadth-first traversal
    std::queue<std::shared_ptr<InventoryComponent>> queue;
    queue.push(component);

    while (!queue.empty()) {
        auto current = queue.front();
        queue.pop();

        // Visit current node
        collection.push_back(current);

        // If this is a Group, enqueue all children
        auto group = std::dynamic_pointer_cast<Group>(current);
        if (group) {
            const auto children = group->members();
            for (const auto& child : children) {
                if (child) {  // Null safety for weak_ptr that expired
                    queue.push(child);
                }
            }
        }
    }
}
