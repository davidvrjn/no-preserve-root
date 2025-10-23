#include "../../../include/Patterns/Iterator/PreOrderTraversal.h"

#include "../../../include/Components/Group.h"
#include "../../../include/Components/InventoryComponent.h"

/**
 * @brief Pre-order traversal: visits root first, then recursively visits children
 * 
 * Algorithm:
 * 1. Add current component to collection
 * 2. If component is a Group, recursively traverse each child
 */
void PreOrderTraversal::traverse(
    const std::shared_ptr<InventoryComponent>& component,
    std::vector<std::shared_ptr<InventoryComponent>>& collection) const {
    
    if (!component) {
        return;  // Null safety
    }

    // Visit root first (pre-order)
    collection.push_back(component);

    // If this is a Group, recursively visit children
    auto group = std::dynamic_pointer_cast<Group>(component);
    if (group) {
        const auto children = group->members();  // Get snapshot of current members
        for (const auto& child : children) {
            traverse(child, collection);  // Recursive call
        }
    }
}
