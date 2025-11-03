
#pragma once
#include <memory>

// Forward declaration
class Observer;

/**
 * @interface Subject
 * @brief The Subject interface for the Observer design pattern.
 *
 * Defines the contract for any object that can be observed. It provides
 * methods for attaching, detaching, and notifying observers. In our system,
 * the Plant class will implement this interface.
 */
class Subject : public std::enable_shared_from_this<Subject> {
   public:
    /**
     * @brief Virtual destructor for proper cleanup of derived classes.
     */
    virtual ~Subject() = default;
    /**
     * @brief Attaches an observer to this subject.
     * @param observer The observer to attach. Implementations should store as weak_ptr.
     */
    virtual void attach(const std::shared_ptr<Observer>& observer) = 0;
    /**
     * @brief Detaches an observer from this subject.
     * @param observer The observer to detach.
     */
    virtual void detach(const std::shared_ptr<Observer>& observer) = 0;
    /**
     * @brief Notifies all attached observers of a state change.
     *
     * Calls update(shared_from_this()) on each observer.
     */
    virtual void notify() = 0;

    /**
     * @brief Removes all attached observers from this subject.
     *
     * Useful for cleanup before destruction to break observer references.
     * Request that the Subject remove all observers (useful before destruction)
     */
    virtual void detachAllObservers() = 0;
};
