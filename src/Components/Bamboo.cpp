#include "../../include/Components/Bamboo.h"

#include "../../include/Core/PlantRegistry.h"
#include "../../include/Patterns/State/PlantState.h"

/**
 * @brief Anonymous namespace for auto-registration of Bamboo plant type
 * 
 * Registers the Bamboo type with PlantRegistry at program startup,
 * enabling factory creation of Bamboo instances.
 */
namespace {
bool registered = []() {
    PlantRegistry::registerType("Bamboo", []() { return std::make_shared<Bamboo>(); });
    return true;
}();
}  // namespace


/**
 * @brief Constructs a Bamboo plant with default characteristics
 * 
 * Initializes a Bamboo plant with the following properties:
 * - Water consumption: 6 units per day (MEDIUM requirement)
 * - Seedling duration: 1 day
 * - Growing duration: 2 days (total growth cycle: 3 days)
 * - Base price: R110.00
 * - Growth season: Year-round availability
 * 
 * Water mechanics: Each watering adds 42% to water level (approximately 6 waterings per day needed)
 */
Bamboo::Bamboo() : Plant("Bamboo", 110.00) {
    setWaterConsumption(6);
    setSeedlingDuration(1);
    setGrowingDuration(2);
    setCharacteristics(WaterRequirement::MEDIUM, {Season::YEAR_ROUND});
}

/**
 * @brief Waters the Bamboo plant
 * 
 * Increases the plant's water level by 42%, reflecting moderate-high water requirements.
 * The water level is capped at 100% to prevent overflow.
 */
void Bamboo::water() {
    int current = getWaterLevel();
    setWaterLevel(std::min(100, current + 42));  // Moderate-high water needs
}

/**
 * @brief Creates a deep copy of this Bamboo instance
 * 
 * @return std::shared_ptr<InventoryComponent> A new Bamboo instance with copied state
 * 
 * Copies all runtime state including:
 * - Unique identifier
 * - Current age
 * - Health status
 * - Current water level
 */
std::shared_ptr<InventoryComponent> Bamboo::clone() const {
    auto cloned = std::make_shared<Bamboo>();
    cloned->setId(getId());
    cloned->setAge(getAge());
    cloned->setHealth(getHealth());
    cloned->setWaterLevel(getWaterLevel());
    return cloned;
}

/**
 * @brief Creates a fresh blueprint instance of Bamboo
 * 
 * @return std::shared_ptr<InventoryComponent> A new Bamboo with default initialization
 * 
 * Used for creating new instances without copying existing state,
 * suitable for shop inventory and plant catalogs.
 */
std::shared_ptr<InventoryComponent> Bamboo::blueprintClone() const {
    return std::make_shared<Bamboo>();
}

/**
 * @brief Serializes the Bamboo plant to JSON format
 * 
 * @return std::string JSON representation including type information and plant state
 * 
 * Extends base Plant serialization by prepending the specific type identifier.
 * Format: {"type":"Bamboo", ...base plant data...}
 */
std::string Bamboo::serialize() const {
    std::string baseJson = Plant::serialize();
    std::string result = "{\"type\":\"Bamboo\",";
    result += baseJson.substr(1);
    return result;
}

/**
 * @brief Deserializes Bamboo plant data from JSON format
 * 
 * @param data JSON string containing serialized plant state
 * 
 * Delegates to base Plant class for parsing all standard plant attributes.
 */
void Bamboo::deserialize(const std::string& data) {
    Plant::deserialize(data);  // Base class handles all fields
}

/**
 * @brief Returns the type name of this plant
 * 
 * @return std::string The string "Bamboo"
 */
std::string Bamboo::typeName() const { return "Bamboo"; }
