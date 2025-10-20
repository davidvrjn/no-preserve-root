#pragma once

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
class Subject {
public:
	virtual ~Subject() {}
	virtual void attach(Observer* observer) = 0;
	virtual void detach(Observer* observer) = 0;
	virtual void notify() = 0;
};

