#pragma once

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
	virtual ~Command() {}
	virtual void execute() = 0;
};

