
#pragma once
#include <memory>

/**
 * @class Customer
 * @brief Represents a customer in the nursery.
 * 
 * In our design, the Customer is a relatively simple actor. Its primary role
 * is to be the originator of a request. The complex logic of what the customer
 * wants is handled by the Nursery (acting as a Director) and the PlantSpecificationBuilder.
 * The Customer object itself is mainly used to link a request to a specific entity.
 */
class Customer : public std::enable_shared_from_this<Customer> {
public:
    Customer();
    ~Customer() = default;
    
    // Customers might have properties like a name or a budget in a more complex simulation.
};
