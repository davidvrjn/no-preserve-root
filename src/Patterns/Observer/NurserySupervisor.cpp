#include "../../../include/Patterns/Observer/NurserySupervisor.h"

NurserySupervisor::NurserySupervisor(const std::shared_ptr<Nursery>& nursery) : nursery(nursery) {}

void NurserySupervisor::update(const std::shared_ptr<Subject>& subject) {
    (void)subject;  // stub: real behavior implemented later
}
