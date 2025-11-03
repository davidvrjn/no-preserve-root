#include "../../include/Actors/Customer.h"

std::atomic<uint64_t> Customer::nextId{5000};

Customer::Customer() : id(++nextId) {}
