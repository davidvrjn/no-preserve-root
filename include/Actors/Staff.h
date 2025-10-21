#pragma once

// Forward declaration
class Command;

/**
 * @class Staff
 * @brief The abstract "Handler" for the Chain of Responsibility pattern.
 * 
 * Defines the common interface for all staff members. It contains the
 * handleRequest method and holds a pointer to the next staff member
 * in the chain (the 'successor'). This allows a request to be passed along
 * until a staff member claims and handles it.
 */
class Staff {
protected:
	Staff* successor;

public:
	Staff();
	virtual ~Staff();

	/**
	 * @brief Sets the next handler in the chain.
	 * @param next The next Staff member in the chain.
	 */
	void setSuccessor(Staff* next);

	/**
	 * @brief The main method for handling a request.
	 * 
	 * Concrete subclasses will implement this to check if they can handle the
	 * command. If not, they will delegate the call to their successor.
	 * @param cmd The Command object to be processed.
	 */
	virtual void handleRequest(Command* cmd) = 0;
};

