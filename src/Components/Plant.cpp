#include "../../include/Components/Plant.h"
#include "../../include/Patterns/State/PlantState.h"
#include "../../include/Patterns/Iterator/Iterator.h"

Plant::Plant(const std::string& name, double price)
	: name(name), price(price), age(0), health(100), waterLevel(100), currentState(nullptr) {}

std::string Plant::getName() const { return name; }

double Plant::getPrice() const { return price; }

std::unique_ptr<Iterator> Plant::createIterator() { return nullptr; }

std::shared_ptr<InventoryComponent> Plant::clone() const { return nullptr; }

std::shared_ptr<InventoryComponent> Plant::blueprintClone() const { return nullptr; }

std::string Plant::serialize() const { return std::string(); }

void Plant::deserialize(const std::string& data) { (void)data; }

std::string Plant::typeName() const { return "Plant"; }

void Plant::setState(std::unique_ptr<PlantState> state) { currentState = std::move(state); }

void Plant::performDailyActivity() { if (currentState) currentState->performDailyActivity(this); }

void Plant::attach(const std::shared_ptr<Observer>& observer) { (void)observer; }

void Plant::detach(const std::shared_ptr<Observer>& observer) { (void)observer; }

void Plant::notify() { /* stub */ }

void Plant::detachAllObservers() { observers.clear(); }

