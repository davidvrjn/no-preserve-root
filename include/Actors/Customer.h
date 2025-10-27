
#pragma once
#include <atomic>
#include <memory>

/**
 * @class Customer
 * @brief Represents a customer in the nursery.
 *
 * The Customer class is intentionally minimal. Its sole purpose is to act as the
 * invoker/creator of a request. Each customer has a unique ID for tracking purposes.
 *
 * All business logic (money, reputation, request fulfillment) is handled by the
 * FulfillCustomerCommand and Nursery classes, not by the Customer itself.
 */
class Customer : public std::enable_shared_from_this<Customer> {
   private:
    static std::atomic<uint64_t> nextId;
    uint64_t id;

   public:
    Customer();
    ~Customer() = default;

    uint64_t getId() const { return id; }
};
