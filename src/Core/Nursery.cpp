#include "../../include/Core/Nursery.h"
#include "../../include/Patterns/Command/Command.h"
#include "../../include/Patterns/Memento/Memento.h"

Nursery::Nursery() : currentDay(0) {}

Nursery::~Nursery() = default;

void Nursery::runSimulation() { }

void Nursery::addRequest(std::unique_ptr<Command> cmd) { (void)cmd; }

Memento* Nursery::createMemento() const { return nullptr; }

void Nursery::restoreFromMemento(Memento* memento) { (void)memento; }

void Nursery::spawnCustomer() { }

void Nursery::processRequestQueue() { }

void Nursery::setupNursery() { }

