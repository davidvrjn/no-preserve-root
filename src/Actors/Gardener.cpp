#include "../../include/Actors/Gardener.h"

#include "../../include/Patterns/Command/Command.h"

Gardener::Gardener() = default;

void Gardener::handleRequest(std::unique_ptr<Command> cmd) {
    (void)cmd;  // stub: real logic implemented later
}
