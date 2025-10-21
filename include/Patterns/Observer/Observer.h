
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
	// Subject is passed as a shared_ptr so observers can safely inspect it without taking ownership.
	virtual void update(const std::shared_ptr<Subject>& subject) = 0;
};

