#pragma once
#include <string>

// Forward declarations
class Memento;
class Nursery;

/**
 * @class SaveSystem
 * @brief The "Caretaker" in the Memento design pattern.
 * 
 * Its responsibility is to manage the saving and loading of Memento objects. It
 * requests a Memento from the Nursery to save it and passes a Memento back to
 * the Nursery to restore state. It knows nothing about the contents of the Memento.
 */
class SaveSystem {
public:
    SaveSystem();
    ~SaveSystem();

    void save(Nursery* nursery, const std::string& filename);
    Memento* load(const std::string& filename);
};
