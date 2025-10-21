#include "../../include/Core/Inventory.h"
#include "../../include/Patterns/Iterator/CompositeIterator.h"

Inventory::Inventory() = default;

Inventory::~Inventory() = default;

void Inventory::add(const std::shared_ptr<InventoryComponent>& component) {
	(void)component; // stub
}

void Inventory::remove(const std::shared_ptr<InventoryComponent>& component) {
	(void)component; // stub
}

std::unique_ptr<Iterator> Inventory::createIterator() {
	return nullptr;
}

