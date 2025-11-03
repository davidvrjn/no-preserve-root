
#pragma once
#include <memory>
#include <string>
#include <vector>

/**
 * @class Memento
 * @brief Memento object for storing Nursery state snapshots.
 *
 * A "dumb" data container implementing the Memento pattern. Stores an
 * immutable snapshot of the Nursery's state without knowledge of the
 * Nursery class itself, maintaining proper encapsulation. Used with
 * Caretaker to implement save/load functionality.
 */
class Memento {
   public:
   /**
     * @struct NurseryState
     * @brief Encapsulates the complete state of a Nursery at a point in time.
     */
    struct NurseryState {
        int day;  // The current day number in the Nursery simulation.
        // TODO: expand with serializable inventory, staff, and other data
        std::string serializedData;  // placeholder for a full serialization blob
    };

   private:
    /**
     * @brief The stored state snapshot.
     */
    NurseryState state;

   public:
   /**
     * @brief Constructs a Memento with the given state snapshot.
     * @param state The Nursery state to store.
     */
    Memento(const NurseryState& state);
    /**
     * @brief Destroy the Memento object
     * 
     */
    ~Memento() = default;
    /**
     * @brief Retrieves the stored state snapshot.
     * @return A copy of the stored NurseryState.
     */
    NurseryState getState() const noexcept;
};
