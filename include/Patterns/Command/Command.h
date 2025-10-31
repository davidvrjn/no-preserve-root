#pragma once

#include <cstdint>
#include <string>

/**
 * @interface Command
 * @brief The interface for the Command design pattern.
 *
 * Encapsulates a request as an object. This interface defines a single 'execute'
 * method. Concrete command classes will implement this method to perform a
 * specific action. This allows us to queue, log, and pass commands around as objects.
 */
class Command {
   public:
    virtual ~Command() = default;
    enum class Status { Pending, Completed, Failed, Cancelled };

    virtual void execute() = 0;

    // Query and set status
    virtual Status getStatus() const = 0;
    virtual void setStatus(Status s) = 0;

    // For serialization: a command may reference a target component by id
    virtual uint64_t getTargetId() const = 0;
    virtual void setTargetId(uint64_t id) = 0;

    /**
     * @brief Human-readable one-line description of this command for UI/logging
     * Default implementation returns a generic name; concrete commands should override.
     */
    virtual std::string toString() const { return std::string("Command"); }
};
