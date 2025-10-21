#pragma once

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
	virtual ~Observer() {}
	virtual void update(Subject* subject) = 0;
};

