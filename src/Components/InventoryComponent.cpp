#include "../../include/Components/InventoryComponent.h"

#include <atomic>

#include "../../include/Components/Group.h"

/**
 * @brief Static atomic counter for generating unique component IDs
 * 
 * Thread-safe counter that ensures each InventoryComponent instance
 * receives a unique identifier across the entire application lifetime.
 * Initialized to 0 and incremented atomically on each construction.
 */
std::atomic<uint64_t> InventoryComponent::nextId{0};

/**
 * @brief Constructs an InventoryComponent with a unique ID
 * 
 * Automatically assigns a unique identifier by atomically incrementing
 * the static nextId counter. This ensures thread-safe ID generation.
 */
InventoryComponent::InventoryComponent() {
    // Assign a unique id at construction time
    id_ = ++nextId;
}

/**
 * @brief Virtual destructor for proper cleanup of derived classes
 */
InventoryComponent::~InventoryComponent() = default;

/**
 * @brief Adds a component to this inventory component
 * 
 * @param component Shared pointer to the component to add
 * 
 * Default implementation does nothing. Composite classes (like Group)
 * override this method to implement container behavior following the
 * Composite design pattern.
 */
void InventoryComponent::add(const std::shared_ptr<InventoryComponent>& component) {
    // Default: do nothing. Composite classes override this.
    (void)component;
}

/**
 * @brief Removes a component from this inventory component
 * 
 * @param component Shared pointer to the component to remove
 * 
 * Default implementation does nothing. Composite classes (like Group)
 * override this method to implement container behavior following the
 * Composite design pattern.
 */
void InventoryComponent::remove(const std::shared_ptr<InventoryComponent>& component) {
    // Default: do nothing. Composite classes override this.
    (void)component;
}

/**
 * @brief Gets the Group that owns this component
 * 
 * @return std::shared_ptr<Group> Shared pointer to the owning Group, or nullptr if no owner
 * 
 * Uses weak_ptr internally to avoid circular references. The weak_ptr is locked
 * to obtain a shared_ptr, which may be null if the owner has been destroyed.
 */
std::shared_ptr<Group> InventoryComponent::getOwner() const { return owner_.lock(); }

/**
 * @brief Sets the Group that owns this component
 * 
 * @param owner Shared pointer to the Group that should own this component
 * 
 * Stores the owner as a weak_ptr to prevent circular reference cycles in the
 * ownership hierarchy, allowing proper memory cleanup.
 */
void InventoryComponent::setOwner(const std::shared_ptr<Group>& owner) { owner_ = owner; }
