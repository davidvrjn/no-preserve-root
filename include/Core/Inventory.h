#pragma once
#include "../Components/InventoryComponent.h"
#include <vector>

/**
 * @class Inventory
 * @brief Manages the collection of all InventoryComponents in the nursery.
 * 
 * This class is the top-level container for our Composite structure. It holds
 * the root-level plants and groups.
 */
class Inventory {
private:
	std::vector<InventoryComponent*> components;

public:
	Inventory();
	~Inventory();

	void add(InventoryComponent* component);
	void remove(InventoryComponent* component);
	Iterator* createIterator(); // Will create a CompositeIterator for the whole inventory.
};
