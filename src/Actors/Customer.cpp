#include "../../include/Actors/Customer.h"

// Initialize static ID counter (start at 5000 to distinguish from plant IDs)
std::atomic<uint64_t> Customer::nextId{5000};

Customer::Customer() : id(++nextId) {}
