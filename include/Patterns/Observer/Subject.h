
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
	virtual ~Subject() = default;
	// Observers are provided as shared_ptr; implementations should store weak_ptrs internally.
	virtual void attach(const std::shared_ptr<Observer>& observer) = 0;
	virtual void detach(const std::shared_ptr<Observer>& observer) = 0;
	// notify() will call update(shared_from_this()) on observers
	virtual void notify() = 0;

	// Request that the Subject remove all observers (useful before destruction)
	virtual void detachAllObservers() = 0;
};

