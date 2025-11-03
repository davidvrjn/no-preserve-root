
#pragma once
#include <memory>
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
 *
 * Extended to provide file I/O operations with JSON persistence.
 */
class SaveSystem {
   public:
    SaveSystem();
    ~SaveSystem() = default;

    /**
     * @brief Save nursery state to a JSON file
     * @param nursery The nursery to save
     * @param filename Path to save file (e.g., "saves/day_10.json")
     */
    void save(const std::shared_ptr<Nursery>& nursery, const std::string& filename);

    /**
     * @brief Load memento from a JSON file
     * @param filename Path to load from
     * @return Memento if successful, nullptr otherwise
     */
    std::unique_ptr<Memento> load(const std::string& filename);

    // --- Utility functions for clients ---

    /**
     * @brief Check if a save file exists
     * @param filename Path to check
     * @return true if file exists and is readable
     */
    static bool fileExists(const std::string& filename);

    /**
     * @brief Validate that a save file contains valid JSON
     * @param filename Path to validate
     * @return true if file contains valid JSON
     */
    static bool validateSaveFile(const std::string& filename);

    /**
     * @brief Get day number from a save file without fully loading it
     * @param filename Path to save file
     * @return Day number, or -1 if error
     */
    static int getSaveDay(const std::string& filename);

    /**
     * @brief Pretty-print JSON content of a save file
     * @param filename Path to save file
     * @return Formatted JSON string, or error message
     */
    static std::string prettyPrintSave(const std::string& filename);
};
