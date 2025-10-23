#include "../../include/Components/Group.h"

#include <algorithm>
#include <memory>

#include "../../include/Patterns/Iterator/CompositeIterator.h"
#include "../../include/Patterns/Iterator/PreOrderTraversal.h"

Group::Group(const std::string& name, bool ownsChildren) : name(name), ownsChildren(ownsChildren) {}

// Composite overrides
std::string Group::getName() const { return name; }

/**
 * @brief Calculates the total price of all components in this group
 * @return Sum of all children's prices (recursively calculated)
 * 
 * Recursively sums the prices of all owned and referenced components.
 * For nested Groups, their getPrice() will also return the sum of their children.
 * 
 * @note When iterating through a Group hierarchy and summing prices:
 * - Don't sum getPrice() on every component (includes Groups, causes double-counting)
 * - Either: Call getPrice() on the root Group directly (recommended)
 * - Or: Filter to only sum leaf nodes (Plants) using FilteredTraversal
 * 
 * @example
 * // Correct: Get total value of a section
 * double total = gardenSection->getPrice();
 * 
 * // Incorrect: Iterating and summing all components
 * auto iter = gardenSection->createIterator();
 * while (iter->hasNext()) {
 *     total += iter->next()->getPrice();  // DON'T DO THIS - double counts Groups
 * }
 * 
 * // Correct: Filter to only sum plants
 * auto plantsOnly = createFilteredIterator(onlyPlants);
 * while (plantsOnly->hasNext()) {
 *     total += plantsOnly->next()->getPrice();  // OK - only leaf nodes
 * }
 */
double Group::getPrice() const {
    double total = 0.0;
    
    // Sum prices of all owned components
    for (const auto& component : ownedComponents) {
        total += component->getPrice();
    }
    
    // Sum prices of all valid referenced components
    for (const auto& weakRef : referencedComponents) {
        if (auto locked = weakRef.lock()) {
            total += locked->getPrice();
        }
    }
    
    return total;
}

/**
 * @brief Creates an iterator to traverse this Group's tree structure
 * @return A CompositeIterator using PreOrderTraversal strategy
 * 
 * By default, uses pre-order traversal (root first, then children).
 * Clients can create custom iterators with different strategies if needed.
 */
std::unique_ptr<Iterator> Group::createIterator() {
    return std::make_unique<CompositeIterator>(
        shared_from_this(),
        std::make_unique<PreOrderTraversal>()
    );
}

/**
 * @brief Creates an iterator with a custom traversal strategy
 * @param strategy The traversal strategy to use (PreOrder, LevelOrder, Filtered, etc.)
 * @return A CompositeIterator using the provided strategy
 * 
 * This allows for creating custom iterators, such as filtered iterators that only
 * return specific types of components (e.g., only Plants, only summer plants, etc.)
 */
std::unique_ptr<Iterator> Group::createIterator(std::unique_ptr<TraversalStrategy> strategy) {
    return std::make_unique<CompositeIterator>(shared_from_this(), std::move(strategy));
}

std::shared_ptr<InventoryComponent> Group::clone() const { return nullptr; }

std::shared_ptr<InventoryComponent> Group::blueprintClone() const { return nullptr; }

std::string Group::serialize() const { return std::string(); }

void Group::deserialize(const std::string& data) { (void)data; }

std::string Group::typeName() const { return "Group"; }

/**
 * @brief Adds a component to this Group
 * 
 * Behavior depends on the ownsChildren flag:
 * - If ownsChildren is true: Stores as owned component and takes ownership
 *   * If component already has an owner, automatically transfers ownership from old owner
 * - If ownsChildren is false: Stores as weak reference (non-owning)
 * 
 * This auto-move simplifies moving plants between plots/storage.
 */
void Group::add(const std::shared_ptr<InventoryComponent>& component) {
    if (!component) {
        return;  // Null safety
    }
    
    if (ownsChildren) {
        // Check if component already has an owner
        auto previousOwner = component->getOwner();
        if (previousOwner) {
            // Auto-move: remove from previous owner first
            auto prevGroup = std::dynamic_pointer_cast<Group>(previousOwner);
            if (prevGroup) {
                prevGroup->remove(component);
            }
        }
        
        // Store as owned component
        ownedComponents.push_back(component);
        // Set this group as the owner
        component->setOwner(shared_from_this());
    } else {
        // Store as weak reference (non-owning)
        referencedComponents.push_back(component);
    }
}

/**
 * @brief Removes a component from this Group
 * 
 * Searches both owned and referenced collections for the component.
 * If found in owned collection, also clears the component's owner.
 * If found in referenced collection, removes the weak_ptr.
 * Does nothing if component is not found or is nullptr.
 */
void Group::remove(const std::shared_ptr<InventoryComponent>& component) {
    if (!component) {
        return;
    }
    
    // Try to remove from owned components
    auto it = std::find(ownedComponents.begin(), ownedComponents.end(), component);
    if (it != ownedComponents.end()) {
        ownedComponents.erase(it);
        component->setOwner(nullptr);
        return;
    }
    
    // Try to remove from referenced components
    referencedComponents.erase(
        std::remove_if(referencedComponents.begin(), referencedComponents.end(),
                      [&component](const std::weak_ptr<InventoryComponent>& weak) {
                          auto locked = weak.lock();
                          return locked == component;
                      }),
        referencedComponents.end()
    );
}

/**
 * @brief Returns a snapshot of all current members
 * 
 * Locks all weak_ptrs and excludes expired entries.
 */
std::vector<std::shared_ptr<InventoryComponent>> Group::members() const {
    std::vector<std::shared_ptr<InventoryComponent>> result;
    
    // Add all owned components
    result.insert(result.end(), ownedComponents.begin(), ownedComponents.end());
    
    // Add all valid referenced components
    for (const auto& weakRef : referencedComponents) {
        if (auto locked = weakRef.lock()) {
            result.push_back(locked);
        }
    }
    
    return result;
}

/**
 * @brief Removes expired weak_ptr references from referencedComponents
 * 
 * This is useful for cleaning up references to deleted components.
 * Called periodically to prevent memory waste from accumulating expired weak_ptrs.
 */
void Group::pruneExpiredReferences() {
    referencedComponents.erase(
        std::remove_if(referencedComponents.begin(), referencedComponents.end(),
                      [](const std::weak_ptr<InventoryComponent>& weak) {
                          return weak.expired();
                      }),
        referencedComponents.end()
    );
}
