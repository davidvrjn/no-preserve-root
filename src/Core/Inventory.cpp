#include "../../include/Core/Inventory.h"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

#include "../../include/Components/Group.h"
#include "../../include/Components/InventoryComponent.h"
#include "../../include/Components/Plant.h"
#include "../../include/Core/PlantRegistry.h"
#include "../../include/Patterns/Iterator/CompositeIterator.h"
#include "../../include/Patterns/Iterator/Iterator.h"
#include "../../include/Patterns/Iterator/PreOrderTraversal.h"
#include "../../include/Patterns/Observer/Subject.h"
#include "../../include/json.hpp"

Inventory::Inventory() = default;

Inventory::~Inventory() {
    // Clear all components and ensure proper cleanup
    // Before removing, detach any observers if components are Plants
    for (auto& component : components) {
        if (component) {
            // If it's a Subject (Plant), detach all observers
            auto subject = std::dynamic_pointer_cast<Subject>(component);

            if (subject) {
                subject->detachAllObservers();
            }

            // Clear the owner reference
            component->setOwner(nullptr);
        }
    }

    components.clear();
}

void Inventory::add(const std::shared_ptr<InventoryComponent>& component) {
    //(void)component;  // stub

    if (!component) {
        return;  // No-op for null components
    }

    // Check if component already exists in inventory
    auto it = std::find(components.begin(), components.end(), component);
    if (it != components.end()) {
        return;  // Already in inventory, no-op
    }

    // Check if component has a previous owner
    auto previousOwner = component->getOwner();
    if (previousOwner) {
        // Auto-move: remove from previous owner
        previousOwner->remove(component);
        // Note: previousOwner->remove() should clear the owner_ field
    }

    // Add to inventory's top-level components
    components.push_back(component);

    // Inventory acts as a top-level container but doesn't use Group's owner mechanism
    // Components at the inventory level are considered "unowned" by any Group
    // (they're owned by the Inventory itself via shared_ptr)
    // We explicitly set owner to nullptr to indicate top-level status
    component->setOwner(nullptr);
    
    // If this component is a plant and we have a callback, invoke it
    auto plant = std::dynamic_pointer_cast<Plant>(component);
    if (plant && onPlantAdded) {
        onPlantAdded(plant);
    }
    
    // If this component is a group, set up callbacks and invoke on all existing plants
    auto grp = std::dynamic_pointer_cast<Group>(component);
    if (grp && onPlantAdded) {
        // Propagate the callback to the group
        grp->setOnPlantAddedCallback(onPlantAdded);
        
        // Get all plants in the group recursively and invoke callback
        auto iter = grp->createIterator();
        while (iter->hasNext()) {
            auto child = iter->next();
            auto childPlant = std::dynamic_pointer_cast<Plant>(child);
            if (childPlant) {
                onPlantAdded(childPlant);
            }
        }
    }
}

void Inventory::setOnPlantAddedCallback(std::function<void(const std::shared_ptr<Plant>&)> callback) {
    onPlantAdded = callback;
    
    // Propagate the callback to all existing groups in the inventory
    for (auto& component : components) {
        auto group = std::dynamic_pointer_cast<Group>(component);
        if (group) {
            group->setOnPlantAddedCallback(callback);
        }
    }
}

void Inventory::remove(const std::shared_ptr<InventoryComponent>& component) {
    //(void)component;  // stub

    if (!component) {
        return;  // No-op for null components
    }

    // Find and remove the component
    auto it = std::find(components.begin(), components.end(), component);

    if (it != components.end()) {
        // Before removing, detach observers if it's a Subject (Plant)
        auto subject = std::dynamic_pointer_cast<Subject>(*it);

        if (subject) {
            subject->detachAllObservers();
        }

        // Clear owner reference
        (*it)->setOwner(nullptr);

        // Remove from vector
        components.erase(it);

        // Note: The component may be destroyed here if this was the last shared_ptr reference
    }
}

std::unique_ptr<Iterator> Inventory::createIterator() {
    // Create a temporary root Group to hold all inventory components for traversal
    // Use ownsChildren=false to avoid modifying component ownership
    auto tempRoot = std::make_shared<Group>("InventoryRoot", false);

    // Add all inventory components as references to the temporary root
    for (const auto& component : components) {
        if (component) {
            tempRoot->add(component);
        }
    }

    // Create and return a CompositeIterator with PreOrderTraversal strategy
    // The iterator will traverse all components in the inventory tree
    return std::make_unique<CompositeIterator>(tempRoot, std::make_unique<PreOrderTraversal>());
}

std::shared_ptr<Group> Inventory::findGroupByName(const std::string& name) {
    for (const auto& component : components) {
        if (component && component->getName() == name) {
            auto group = std::dynamic_pointer_cast<Group>(component);
            if (group) {
                return group;
            }
        }
    }
    return nullptr;
}

// Helper function to recursively serialize owned children of a group
static void serializeOwnedChildren(const std::shared_ptr<Group>& group, std::ostringstream& json) {
    if (!group || !group->owns()) {
        return;  // Skip non-owning groups
    }

    std::cerr << "  serializeOwnedChildren for group " << group->getName() << " (id=" << group->getId() << "), ownedComponents.size()=" << group->getOwnedComponents().size() << "\n";
    for (const auto& child : group->getOwnedComponents()) {
        if (child) {
            std::cerr << "    Serializing owned child: " << child->getId() << " (" << child->getName() << ")\n";
            json << "," << child->serialize();

            // Recursively handle nested owning groups
            auto childGroup = std::dynamic_pointer_cast<Group>(child);
            if (childGroup) {
                serializeOwnedChildren(childGroup, json);
            }
        }
    }
}

std::string Inventory::serialize() const {
    std::ostringstream json;
    json << "{";

    json << "\"components\":[";

    // Serialize all top-level components and their children
    bool first = true;
    for (const auto& component : components) {
        if (component) {
            if (!first) json << ",";
            json << component->serialize();
            first = false;

            // If this is an owning Group, recursively serialize its OWNED children
            // (Referenced components are serialized separately as they're top-level or owned
            // elsewhere)
            auto group = std::dynamic_pointer_cast<Group>(component);
            if (group) {
                serializeOwnedChildren(group, json);
            }
        }
    }

    json << "]";
    json << "}";
    return json.str();
}

void Inventory::deserialize(const std::string& data) {
    // Parse JSON
    auto json = nlohmann::json::parse(data);

    // Clear existing components
    components.clear();

    // Two-phase deserialization:
    // Phase 1: Create all components and build ID registry
    std::unordered_map<uint64_t, std::shared_ptr<InventoryComponent>> registry;

    for (const auto& compJson : json["components"]) {
        std::string type = compJson["type"].get<std::string>();
        std::shared_ptr<InventoryComponent> component;

        if (type == "Group") {
            component = std::make_shared<Group>("temp");  // Name will be overwritten by deserialize
        } else {
            // Use PlantRegistry to create the correct plant type
            std::shared_ptr<Plant> plant = PlantRegistry::create(type);
            component = std::shared_ptr<InventoryComponent>(plant);  // Explicit cast
        }

        // Deserialize the component (restores all fields including ID)
        component->deserialize(compJson.dump());

        // Register in ID map
        registry[component->getId()] = component;
    }

    // Phase 2: Resolve all Group references
    for (const auto& [id, component] : registry) {
        auto group = std::dynamic_pointer_cast<Group>(component);
        if (group) {
            // Resolve owned components
            for (uint64_t childId : group->getPendingOwnedIds()) {
                auto it = registry.find(childId);
                if (it != registry.end()) {
                    group->add(it->second);
                }
            }

            // Resolve referenced components
            for (uint64_t refId : group->getPendingReferencedIds()) {
                auto it = registry.find(refId);
                if (it != registry.end()) {
                    // For referenced components, we need to add them as non-owning references
                    // This is handled by Group::add() based on ownsChildren flag
                    // But we need to ensure the group doesn't own these
                    // The group was already configured with ownsChildren during its deserialize()
                    group->add(it->second);
                }
            }
        }
    }

    // Phase 3: Add top-level components to inventory
    // Top-level components are those without an owner
    for (const auto& [id, component] : registry) {
        if (!component->getOwner()) {
            add(component);
        }
    }
}

int Inventory::countAllComponents() const {
    std::unordered_set<uint64_t> visited;
    auto iter = const_cast<Inventory*>(this)->createIterator();
    while (iter->hasNext()) {
        auto component = iter->next();
        if (component) {
            visited.insert(component->getId());
        }
    }
    return static_cast<int>(visited.size());
}

int Inventory::countByType(const std::string& typeName) const {
    std::unordered_set<uint64_t> visited;
    int count = 0;
    auto iter = const_cast<Inventory*>(this)->createIterator();
    while (iter->hasNext()) {
        auto component = iter->next();
        if (component && visited.find(component->getId()) == visited.end()) {
            visited.insert(component->getId());
            if (component->typeName() == typeName) {
                count++;
            }
        }
    }
    return count;
}

std::vector<std::shared_ptr<InventoryComponent>> Inventory::findAll(
    std::function<bool(const std::shared_ptr<InventoryComponent>&)> predicate) const {
    std::unordered_set<uint64_t> visited;
    std::vector<std::shared_ptr<InventoryComponent>> results;
    auto iter = const_cast<Inventory*>(this)->createIterator();
    while (iter->hasNext()) {
        auto component = iter->next();
        if (component && visited.find(component->getId()) == visited.end()) {
            visited.insert(component->getId());
            if (predicate(component)) {
                results.push_back(component);
            }
        }
    }
    return results;
}

std::vector<std::shared_ptr<Plant>> Inventory::getAllPlants() const {
    std::unordered_set<uint64_t> visited;
    std::vector<std::shared_ptr<Plant>> plants;
    auto iter = const_cast<Inventory*>(this)->createIterator();
    while (iter->hasNext()) {
        auto component = iter->next();
        if (component && visited.find(component->getId()) == visited.end()) {
            visited.insert(component->getId());
            auto plant = std::dynamic_pointer_cast<Plant>(component);
            if (plant) {
                plants.push_back(plant);
            }
        }
    }
    return plants;
}

std::vector<std::shared_ptr<Group>> Inventory::getAllGroups() const {
    std::vector<std::shared_ptr<Group>> groups;
    auto iter = const_cast<Inventory*>(this)->createIterator();
    while (iter->hasNext()) {
        auto component = iter->next();
        if (component) {
            auto group = std::dynamic_pointer_cast<Group>(component);
            if (group) {
                groups.push_back(group);
            }
        }
    }
    return groups;
}
