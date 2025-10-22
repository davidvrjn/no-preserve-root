
#pragma once
#include <memory>
#include <vector>

#include "InventoryComponent.h"

/**
 * @class Group
 * @brief Represents a collection of InventoryComponents (a "Composite" in the pattern).
 *
 * This class can hold other InventoryComponents, including other Groups, to form
 * a tree structure. Groups may either own their children (owning collection)
 * or hold non-owning references to components that are owned elsewhere (reference collection).
 * This supports "view" groups like 'complete inventory' that must not take ownership.
 */

class Group : public InventoryComponent, public std::enable_shared_from_this<Group> {
   private:
    std::string name;
    // Flag: when true the group owns added children (add() will store shared_ptrs).
    // When false the group will treat add() as a non-owning reference (stores weak_ptrs).
    bool ownsChildren;

    // Owned children: this group is responsible for the lifetime of these components.
    std::vector<std::shared_ptr<InventoryComponent>> ownedComponents;

    // Non-owning references to components (weak_ptr to avoid dangling owning cycles).
    std::vector<std::weak_ptr<InventoryComponent>> referencedComponents;

   public:
    // ownsChildren indicates whether this group takes ownership of added components
    Group(const std::string& name, bool ownsChildren = true);
    ~Group() override = default;

    // --- Overrides from InventoryComponent (Composite & Prototype) ---
    std::string getName() const override;
    double getPrice() const override;                     // Will sum the prices of its children.
    std::unique_ptr<Iterator> createIterator() override;  // Will create a CompositeIterator.
    std::shared_ptr<InventoryComponent> clone()
        const override;  // Will perform a deep copy of owned children.
    std::shared_ptr<InventoryComponent> blueprintClone() const override;
    std::string serialize() const override;
    void deserialize(const std::string& data) override;
    std::string typeName() const override;

    // --- Composite-specific methods ---
    // Adds a component. Behavior depends on the group's 'ownsChildren' flag:
    // - if ownsChildren == true, the component will be stored as a shared_ptr and owned by this
    // group.
    // - if ownsChildren == false, the component will be referenced weakly and not owned.
    //   * IMPORTANT: If the component already has an owner (component->getOwner() != nullptr),
    //     calling add() on an owning group will transfer ownership to this group (auto-move).
    //     The expected implementation steps are:
    //       1. Check component->getOwner(); if non-null, call previousOwner->remove(component) to
    //       detach it.
    //       2. Insert the component into this group's ownedComponents and call
    //       component->setOwner(shared_from_this()).
    //       3. Ensure invariants (component now has exactly one owner).
    //   This auto-move keeps ownership deterministic and simplifies moving plants between
    //   plots/storage.

    void add(const std::shared_ptr<InventoryComponent>& component) override;

    // Remove component from either owned or referenced lists
    void remove(const std::shared_ptr<InventoryComponent>& component) override;

    // Returns whether this Group owns children added to it.
    // noexcept: trivial check of an internal flag.
    bool owns() const noexcept { return ownsChildren; }

    // Returns a snapshot list of current members (locks weak_ptrs and excludes expired entries).
    // Note: This returns a fresh vector of shared_ptrs and does not mutate this Group.
    std::vector<std::shared_ptr<InventoryComponent>> members() const;

    // Prune expired weak references from referencedComponents.
    void pruneExpiredReferences();
};
