#include "../../include/Actors/Customer.h"

/**
 * @brief Static atomic counter for generating unique customer IDs
 * 
 * Thread-safe counter initialized to 5000 to distinguish customer IDs from plant IDs.
 * This ID range separation helps prevent collisions and makes debugging easier by
 * clearly identifying customer-related objects in the system.
 */
std::atomic<uint64_t> Customer::nextId{5000};

/**
 * @brief Constructs a new Customer with a unique ID
 * 
 * Automatically assigns a unique identifier by atomically incrementing the static
 * nextId counter. Customer IDs start at 5000 to maintain a separate ID namespace
 * from plants (which start at 0).
 */
Customer::Customer() : id(++nextId) {}
