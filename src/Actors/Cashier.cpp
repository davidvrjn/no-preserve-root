#include "../../include/Actors/Cashier.h"
#include "../../include/Patterns/Command/Command.h"

Cashier::Cashier() = default;

void Cashier::handleRequest(std::unique_ptr<Command> cmd) {
	(void)cmd; // stub
}

