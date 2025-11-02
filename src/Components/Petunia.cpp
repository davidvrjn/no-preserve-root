#include "../../include/Components/Petunia.h"

#include "../../include/Core/PlantRegistry.h"
#include "../../include/Patterns/State/PlantState.h"

/**
 * @brief Anonymous namespace for auto-registration of Petunia plant type
 * 
 * Registers the Petunia type with PlantRegistry at program startup,
 * enabling factory creation of Petunia instances.
 */
namespace {
bool registered = []() {
    PlantRegistry::registerType("Petunia", []() { return std::make_shared<Petunia>(); });
    return true;
}();
}  // namespace

/**
 * @brief Constructs a Petunia plant with default characteristics
 * 
 * Initializes a Petunia plant with the following properties:
 * - Water consumption: 5 units per day (MEDIUM requirement)
 * - Seedling duration: 2 days
 * - Growing duration: 3 days (total growth cycle: 5 days)
 * - Base price: R110.00
 * - Growth season: Spring, Summer, and Fall
 * 
 * Water mechanics: Each watering adds 40% to water level (approximately 5 waterings per day needed)
 */
Petunia::Petunia() : Plant("Petunia", 110.00) {
    setWaterConsumption(5);
    setSeedlingDuration(2);
    setGrowingDuration(3);
    setCharacteristics(WaterRequirement::MEDIUM, {Season::SPRING, Season::SUMMER, Season::FALL});
}

/**
 * @brief Waters the Petunia plant
 * 
 * Increases the plant's water level by 40%, reflecting moderate water requirements.
 * The water level is capped at 100% to prevent overflow.
 */
void Petunia::water() {
    int current = getWaterLevel();
    setWaterLevel(std::min(100, current + 40));  // Moderate water needs
}

/**
 * @brief Creates a deep copy of this Petunia instance
 * 
 * @return std::shared_ptr<InventoryComponent> A new Petunia instance with copied state
 * 
 * Copies all runtime state including:
 * - Unique identifier
 * - Current age
 * - Health status
 * - Current water level
 */

std::shared_ptr<InventoryComponent> Petunia::clone() const {
    auto cloned = std::make_shared<Petunia>();
    cloned->setId(getId());
    cloned->setAge(getAge());
    cloned->setHealth(getHealth());
    cloned->setWaterLevel(getWaterLevel());
    return cloned;
}

/**
 * @brief Creates a fresh blueprint instance of Petunia
 * 
 * @return std::shared_ptr<InventoryComponent> A new Petunia with default initialization
 * 
 * Used for creating new instances without copying existing state,
 * suitable for shop inventory and plant catalogs.
 */
std::shared_ptr<InventoryComponent> Petunia::blueprintClone() const {
    return std::make_shared<Petunia>();
}

/**
 * @brief Serializes the Petunia plant to JSON format
 * 
 * @return std::string JSON representation including type information and plant state
 * 
 * Extends base Plant serialization by prepending the specific type identifier.
 * Format: {"type":"Petunia", ...base plant data...}
 */
std::string Petunia::serialize() const {
    std::string baseJson = Plant::serialize();
    std::string result = "{\"type\":\"Petunia\",";
    result += baseJson.substr(1);
    return result;
}

/**
 * @brief Deserializes Petunia plant data from JSON format
 * 
 * @param data JSON string containing serialized plant state
 * 
 * Delegates to base Plant class for parsing all standard plant attributes.
 */
void Petunia::deserialize(const std::string& data) {
    Plant::deserialize(data);  // Base class handles all fields
}

/**
 * @brief Returns the type name of this plant
 * 
 * @return std::string The string "Petunia"
 */
std::string Petunia::typeName() const { return "Petunia"; }
