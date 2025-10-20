#pragma once
#include <string>
#include <vector>

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
class InventoryComponent {
public:
	InventoryComponent();
	virtual ~InventoryComponent();

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
	 * @return A pointer to an Iterator object.
	 */
	virtual Iterator* createIterator() = 0;

	/**
	 * @brief (Prototype Pattern) Creates a deep copy of this component.
	 *
	 * This method is the core of the Prototype pattern. It allows for the
	 * creation of a new, independent copy of any inventory item, including
	 * entire groups with all their children.
	 * @return A pointer to a new InventoryComponent instance.
	 */
	virtual InventoryComponent* clone() const = 0;

	// --- Methods for the Composite structure ---
	// Default implementations do nothing or throw errors for Leaf objects (Plant).
    
	/**
	 * @brief Adds a component to this component's collection.
	 * @param component The component to add.
	 */
	virtual void add(InventoryComponent* component);

	/**
	 * @brief Removes a component from this component's collection.
	 * @param component The component to remove.
	 */
	virtual void remove(InventoryComponent* component);
};

