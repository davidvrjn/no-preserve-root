#pragma once
#include "InventoryComponent.h"
#include <vector>

/**
 * @class Group
 * @brief Represents a collection of InventoryComponents (a "Composite" in the pattern).
 *
 * This class can hold other InventoryComponents, including other Groups, to form
 * a tree structure. This is used to model plots, greenhouse sections, or logical
 * groupings like "ReadyToSell". It provides implementations for `add` and `remove`
 * and overrides other methods to operate on its children.
 */
class Group : public InventoryComponent {
private:
	std::string name;
	std::vector<InventoryComponent*> components;

public:
	Group(const std::string& name);
	virtual ~Group();

	// --- Overrides from InventoryComponent (Composite & Prototype) ---
	std::string getName() const override;
	double getPrice() const override; // Will sum the prices of its children.
	Iterator* createIterator() override; // Will create a CompositeIterator.
	InventoryComponent* clone() const override; // Will perform a deep copy.

	// --- Composite-specific methods ---
	void add(InventoryComponent* component) override;
	void remove(InventoryComponent* component) override;
};
