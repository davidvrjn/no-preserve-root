
#pragma once
#include <memory>

// Forward declaration
class Subject;

/**
 * @interface Observer
 * @brief The Observer interface for the Observer design pattern.
 *
 * Defines the contract for any object that can observe a Subject. It has a single
 * update method that is called by the Subject when its state changes.
 */
class Observer {
   public:
    virtual ~Observer() = default;
    /**
     * @brief Called when the observed Subject's state changes.
     * 
     * The Subject is passed as shared_ptr so observers can safely inspect
     * it without taking ownership.
     * 
     * @param subject The Subject that changed and triggered this notification.
     */
    virtual void update(const std::shared_ptr<Subject>& subject) = 0;
};
