#include "../../include/Components/Group.h"

#include <algorithm>
#include <memory>

#include "../../include/Patterns/Iterator/CompositeIterator.h"
#include "../../include/Patterns/Iterator/PreOrderTraversal.h"

Group::Group(const std::string& name, bool ownsChildren) : name(name), ownsChildren(ownsChildren) {}

// Composite overrides - minimal stubs
std::string Group::getName() const { return name; }

double Group::getPrice() const {
    double total = 0.0;
    // Minimal stub: not traversing children; concrete implementation required later.
    (void)total;
    return 0.0;
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
 * If ownsChildren is true, stores as owned component.
 * If ownsChildren is false, stores as weak reference.
 */
void Group::add(const std::shared_ptr<InventoryComponent>& component) {
    if (!component) {
        return;  // Null safety
    }
    
    if (ownsChildren) {
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

void Group::pruneExpiredReferences() {}
