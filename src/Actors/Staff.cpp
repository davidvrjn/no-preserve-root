#include "../../include/Actors/Staff.h"

Staff::Staff() = default;

void Staff::setSuccessor(const std::shared_ptr<Staff>& next) noexcept { successor = next; }
