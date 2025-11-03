#include "../../include/Actors/Staff.h"

#include "../../include/Patterns/Command/Command.h"

Staff::Staff() : successor(nullptr), busy(false) {}

void Staff::setSuccessor(const std::shared_ptr<Staff>& next) noexcept { successor = next; }
