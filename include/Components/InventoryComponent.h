
#pragma once
#include <string>
#include <vector>
#include <memory>
#include <cstdint>
#include <atomic>

// Forward declaration to break circular dependency with Iterator.
class Iterator;

/**
 * @class InventoryComponent
 * @brief The abstract base class for all items in the nursery's inventory.
 *
 * This class is the cornerstone of the Composite design pattern. It defines the
 * common interface that both individual items (Leafs, like Plant) and collections
 * of items (Composites, like Group) must implement. This allows clients to treat
 * all inventory items uniformly.
 *
 * It also serves as the base for the Prototype pattern by defining a pure virtual
 * 'clone()' method, forcing all subclasses to be cloneable.
 */
// Forward declare Group to allow owner tracking.
class Group;

class InventoryComponent {
public:
	InventoryComponent();
	virtual ~InventoryComponent();

	// Unique identifier used for serialization and reference resolution.
	uint64_t getId() const noexcept { return id_; }
	void setId(uint64_t id) noexcept { id_ = id; }

	/**
	 * @brief Gets the name of the inventory component.
	 * @return A string representing the component's name.
	 */
	virtual std::string getName() const = 0;

	/**
	 * @brief Gets the price of the inventory component.
	 *
	 * For a single Plant, this is its base price. For a Group, this would be
	 * the sum of the prices of all its children.
	 * @return A double representing the price.
	 */
	virtual double getPrice() const = 0;

	/**
	 * @brief Creates an iterator to traverse this component.
	 *
	 * For a Plant, the iterator will be simple. For a Group, this will
	 * create a complex iterator (like CompositeIterator) capable of traversing
	 * the entire subtree.
	 * @return A unique_ptr to an Iterator object (caller owns it).
	 */
	// Create an iterator to traverse this component. Implementations may or may not
	// mutate internal transient iteration state; keep non-const to match existing .cpps.
	virtual std::unique_ptr<Iterator> createIterator() = 0;

	/**
	 * @brief (Prototype Pattern) Creates a deep copy of this component.
	 *
	 * This method is the core of the Prototype pattern. It allows for the
	 * creation of a new, independent copy of any inventory item, including
	 * entire groups with all their children.
	 * @return A shared_ptr to a new InventoryComponent instance.
	 */
	/**
	 * @brief (Prototype Pattern) Creates a deep copy of this component.
	 *
	 * IMPORTANT: In this design clone() is intended for snapshotting (Memento).
	 * A clone used for a snapshot should preserve the component's ID and runtime state
	 * so that the Memento can restore the system exactly as it was. Do NOT use clone()
	 * when creating user-facing "blueprints"; use blueprintClone() for that.
	 * @return A shared_ptr to a new InventoryComponent instance which preserves ID/state.
	 */
	virtual std::shared_ptr<InventoryComponent> clone() const = 0;

	/**
	 * @brief Creates a blueprint-style clone: a fresh component with default runtime fields and a new id.
	 *
	 * This is intended for user-driven copying of plots/blueprints (not for Memento snapshots).
	 */
	virtual std::shared_ptr<InventoryComponent> blueprintClone() const = 0;

	// Serialization hooks (JSON text). Implementations should return a JSON string representing the object.
	virtual std::string serialize() const = 0;
	virtual void deserialize(const std::string& data) = 0;

	// --- Methods for the Composite structure ---
	// Default implementations do nothing or throw errors for Leaf objects (Plant).

	/**
	 * @brief Adds a component to this component's collection (owned relationship).
	 * @param component The component to add (shared ownership).
	 */
	virtual void add(const std::shared_ptr<InventoryComponent>& component);

	/**
	 * @brief Removes a component from this component's collection.
	 * @param component The component to remove.
	 */
	virtual void remove(const std::shared_ptr<InventoryComponent>& component);
    
	// Returns the human-readable type name, used during serialization/deserialization
	virtual std::string typeName() const = 0;

	// Owner tracking (single-owner invariant): returns the owning Group if any.
	std::shared_ptr<Group> getOwner() const;
	void setOwner(const std::shared_ptr<Group>& owner);
protected:
	uint64_t id_{0};
	// next id generator. Definition of nextId must be provided in a .cpp file.
	static std::atomic<uint64_t> nextId;

	// Owner: when set, this component is owned by exactly one Group. Use weak_ptr to avoid cycles.
	std::weak_ptr<Group> owner_;
};


