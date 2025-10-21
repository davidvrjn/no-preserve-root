
#pragma once
#include <string>
#include <vector>
#include <memory>

/**
 * @class Memento
 * @brief The Memento object for the Memento design pattern.
 * 
 * This is a "dumb" data container. Its only job is to store a snapshot of the
 * Nursery's state. It provides a getter for the state but should have no knowledge
 * of the Nursery class itself to maintain encapsulation.
 */
class Memento {
public:
	struct NurseryState {
		int day;
		// TODO: expand with serializable inventory, staff, and other data
		std::string serializedData; // placeholder for a full serialization blob
	};

private:
	NurseryState state;

public:
	Memento(const NurseryState& state);
	~Memento() = default;

	NurseryState getState() const noexcept;
};
