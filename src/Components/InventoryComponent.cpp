#include "../../include/Components/InventoryComponent.h"

#include <atomic>

#include "../../include/Components/Group.h"

// Define and initialize the static nextId counter
std::atomic<uint64_t> InventoryComponent::nextId{0};

InventoryComponent::InventoryComponent() {
    // Assign a unique id at construction time
    id_ = ++nextId;
}

InventoryComponent::~InventoryComponent() = default;

void InventoryComponent::add(const std::shared_ptr<InventoryComponent>& component) {
    // Default: do nothing. Composite classes override this.
    (void)component;
}

void InventoryComponent::remove(const std::shared_ptr<InventoryComponent>& component) {
    // Default: do nothing. Composite classes override this.
    (void)component;
}

std::shared_ptr<Group> InventoryComponent::getOwner() const { return owner_.lock(); }

void InventoryComponent::setOwner(const std::shared_ptr<Group>& owner) { owner_ = owner; }
