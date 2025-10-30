#include "../../include/Components/Group.h"

#include <algorithm>
#include <memory>
#include <sstream>

#include "../../include/Patterns/Iterator/CompositeIterator.h"
#include "../../include/Patterns/Iterator/PreOrderTraversal.h"
#include "../../include/Patterns/Observer/Subject.h"
#include "../../include/json.hpp"

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
    return std::make_unique<CompositeIterator>(shared_from_this(),
                                               std::make_unique<PreOrderTraversal>());
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

/**
 * @brief Creates a deep copy of this Group and all owned children
 *
 * This performs a full recursive clone for Memento/save-load purposes.
 * - Clones the Group itself (preserving name, ownsChildren flag, ID)
 * - Recursively clones all owned children (deep copy)
 * - Does NOT clone referenced children (they're just references)
 * - Preserves IDs so the clone can restore exact state
 *
 * @return A new Group with cloned owned children
 */
std::shared_ptr<InventoryComponent> Group::clone() const {
    // Create new group with same properties
    auto cloned = std::make_shared<Group>(name, ownsChildren);
    cloned->setId(getId());  // Preserve ID for state restoration

    // Deep copy all owned children
    for (const auto& child : ownedComponents) {
        if (child) {
            auto clonedChild = child->clone();
            if (clonedChild) {
                cloned->ownedComponents.push_back(clonedChild);
                clonedChild->setOwner(cloned);  // Update owner reference
            }
        }
    }

    // Note: We don't clone referenced children because they're non-owning references
    // They would need to be re-established after restoration from memento

    return cloned;
}

/**
 * @brief Creates a fresh blueprint copy of this Group
 *
 * Unlike clone(), this creates a "clean" copy for user-facing features like
 * duplicating plot layouts. It:
 * - Creates a new Group with same structure
 * - Recursively blueprintClones all owned children (fresh copies, new IDs)
 * - Does NOT preserve IDs (gets new IDs automatically)
 * - Does NOT clone referenced children (they're just references)
 *
 * This is useful for "Save Plot as Template" features where users want
 * a fresh copy of a layout without the original state.
 *
 * @return A fresh Group with blueprint-cloned children
 */
std::shared_ptr<InventoryComponent> Group::blueprintClone() const {
    // Create new group with same properties (gets new ID automatically)
    auto cloned = std::make_shared<Group>(name, ownsChildren);

    // Blueprint clone all owned children (fresh copies)
    for (const auto& child : ownedComponents) {
        if (child) {
            auto clonedChild = child->blueprintClone();
            if (clonedChild) {
                cloned->ownedComponents.push_back(clonedChild);
                clonedChild->setOwner(cloned);  // Update owner reference
            }
        }
    }

    // Note: Referenced children are not cloned (non-owning references)

    return cloned;
}

std::string Group::serialize() const {
    std::ostringstream json;
    json << "{";

    // Group metadata
    json << "\"type\":\"Group\",";
    json << "\"id\":" << getId() << ",";
    json << "\"name\":\"" << name << "\",";
    json << "\"ownsChildren\":" << (ownsChildren ? "true" : "false") << ",";

    // Owned components - store only IDs (actual serialization happens elsewhere)
    json << "\"ownedComponents\":[";
    for (size_t i = 0; i < ownedComponents.size(); ++i) {
        if (i > 0) json << ",";
        json << ownedComponents[i]->getId();
    }
    json << "],";

    // Referenced components - store only IDs of currently valid references
    json << "\"referencedComponents\":[";
    bool first = true;
    for (const auto& weakRef : referencedComponents) {
        if (auto locked = weakRef.lock()) {
            if (!first) json << ",";
            json << locked->getId();
            first = false;
        }
    }
    json << "]";

    json << "}";
    return json.str();
}

void Group::deserialize(const std::string& data) {
    // Parse JSON
    auto json = nlohmann::json::parse(data);

    // Restore ID to preserve original
    setId(json["id"].get<uint64_t>());

    // Restore metadata
    name = json["name"].get<std::string>();
    ownsChildren = json["ownsChildren"].get<bool>();

    // Store component IDs for phase 2 resolution
    // Don't resolve them yet - Inventory will do this after all components are created
    pendingOwnedIds.clear();
    for (const auto& idJson : json["ownedComponents"]) {
        pendingOwnedIds.push_back(idJson.get<uint64_t>());
    }

    pendingReferencedIds.clear();
    for (const auto& idJson : json["referencedComponents"]) {
        pendingReferencedIds.push_back(idJson.get<uint64_t>());
    }
}

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
        // If it's a Subject (Plant), detach all observers before removing
        auto subject = std::dynamic_pointer_cast<Subject>(component);
        if (subject) {
            subject->detachAllObservers();
        }
        
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
        referencedComponents.end());
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
        std::remove_if(
            referencedComponents.begin(), referencedComponents.end(),
            [](const std::weak_ptr<InventoryComponent>& weak) { return weak.expired(); }),
        referencedComponents.end());
}
