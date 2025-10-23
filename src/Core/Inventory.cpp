#include "../../include/Core/Inventory.h"

#include "../../include/Patterns/Iterator/CompositeIterator.h"
#include "../../include/Patterns/Iterator/PreOrderTraversal.h"
#include "../../include/Patterns/Observer/Subject.h"
#include "../../include/Components/InventoryComponent.h"
#include "../../include/Components/Group.h"

#include <algorithm>

Inventory::Inventory() = default;

Inventory::~Inventory()
{
    // Clear all components and ensure proper cleanup
    // Before removing, detach any observers if components are Plants
    for(auto& component : components)
    {
        if(component)
        {
            // If it's a Subject (Plant), detach all observers
            auto subject = std::dynamic_pointer_cast<Subject>(component);
            
            if(subject)
            {
                subject->detachAllObservers();
            }

            // Clear the owner reference
            component->setOwner(nullptr);
        }
    }
    
    components.clear();
}

void Inventory::add(const std::shared_ptr<InventoryComponent>& component)
{
    //(void)component;  // stub

    if(!component)
    {
        return;  // No-op for null components
    }

    // Check if component already exists in inventory
    auto it = std::find(components.begin(), components.end(), component);
    if(it != components.end())
    {
        return;  // Already in inventory, no-op
    }

    // Check if component has a previous owner
    auto previousOwner = component->getOwner();
    if(previousOwner)
    {
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
}

void Inventory::remove(const std::shared_ptr<InventoryComponent>& component)
{
    //(void)component;  // stub
    
    if(!component)
    {
        return;  // No-op for null components
    }

    // Find and remove the component
    auto it = std::find(components.begin(), components.end(), component);

    if(it != components.end())
    {
        // Before removing, detach observers if it's a Subject (Plant)
        auto subject = std::dynamic_pointer_cast<Subject>(*it);

        if(subject)
        {
            subject->detachAllObservers();
        }

        // Clear owner reference
        (*it)->setOwner(nullptr);

        // Remove from vector
        components.erase(it);
        
        // Note: The component may be destroyed here if this was the last shared_ptr reference
    }
}

std::unique_ptr<Iterator> Inventory::createIterator()
{ 
    // Create a temporary root Group to hold all inventory components for traversal
    // This is a non-owning reference group (ownsChildren = false)
    auto tempRoot = std::make_shared<Group>("InventoryRoot", false);
    
    // Add all inventory components as references to the temporary root
    for(const auto& component : components)
    {
        if(component)
        {
            tempRoot->add(component);
        }
    }

    // Create and return a CompositeIterator with PreOrderTraversal strategy
    // The iterator will traverse all components in the inventory tree
    return std::make_unique<CompositeIterator>(tempRoot, std::make_unique<PreOrderTraversal>());
}
