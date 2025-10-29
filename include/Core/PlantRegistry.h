#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

// Forward declaration
class Plant;

/**
 * @brief Registry for creating Plant objects by type string
 * 
 * This enables deserialization to create the correct concrete Plant type
 * based on the "type" field in JSON. Plant types auto-register themselves
 * using static initialization in their .cpp files.
 */
class PlantRegistry {
   private:
    static std::unordered_map<std::string, std::function<std::shared_ptr<Plant>()>>& getRegistry();

   public:
    /**
     * @brief Register a plant type with its factory function
     * @param type Type string (e.g., "Rose", "Cactus")
     * @param factory Function that creates a new instance
     */
    static void registerType(const std::string& type,
                            std::function<std::shared_ptr<Plant>()> factory);

    /**
     * @brief Create a Plant instance by type string
     * @param type Type string matching a registered type
     * @return shared_ptr to newly created Plant
     * @throws std::out_of_range if type is not registered
     */
    static std::shared_ptr<Plant> create(const std::string& type);

    /**
     * @brief Check if a type is registered
     * @param type Type string to check
     * @return true if registered, false otherwise
     */
    static bool isRegistered(const std::string& type);
};
