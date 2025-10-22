#include "../../include/Components/Group.h"

#include <algorithm>

#include "../../include/Patterns/Iterator/CompositeIterator.h"

Group::Group(const std::string& name, bool ownsChildren) : name(name), ownsChildren(ownsChildren) {}

// Composite overrides - minimal stubs
std::string Group::getName() const { return name; }

double Group::getPrice() const {
    double total = 0.0;
    // Minimal stub: not traversing children; concrete implementation required later.
    (void)total;
    return 0.0;
}

std::unique_ptr<Iterator> Group::createIterator() { return nullptr; }

std::shared_ptr<InventoryComponent> Group::clone() const { return nullptr; }

std::shared_ptr<InventoryComponent> Group::blueprintClone() const { return nullptr; }

std::string Group::serialize() const { return std::string(); }

void Group::deserialize(const std::string& data) { (void)data; }

std::string Group::typeName() const { return "Group"; }

void Group::add(const std::shared_ptr<InventoryComponent>& component) {
    (void)component;  // stub - does nothing
}

void Group::remove(const std::shared_ptr<InventoryComponent>& component) {
    (void)component;  // stub - does nothing
}

std::vector<std::shared_ptr<InventoryComponent>> Group::members() const { return {}; }

void Group::pruneExpiredReferences() {}
