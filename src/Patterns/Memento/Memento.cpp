#include "../../../include/Patterns/Memento/Memento.h"

Memento::Memento(const NurseryState& state) : state(state) {}

Memento::NurseryState Memento::getState() const noexcept { return state; }

