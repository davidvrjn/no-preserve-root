#include "../../include/Components/Ivy.h"

#include "../../include/Core/PlantRegistry.h"
#include "../../include/Patterns/State/PlantState.h"

/**
 * @brief Anonymous namespace for auto-registration of Ivy plant type
 * 
 * Registers the Ivy type with PlantRegistry at program startup,
 * enabling factory creation of Ivy instances.
 */
namespace {
bool registered = []() {
    PlantRegistry::registerType("Ivy", []() { return std::make_shared<Ivy>(); });
    return true;
}();
}  // namespace

/**
 * @brief Constructs an Ivy plant with default characteristics
 * 
 * Initializes an Ivy plant with the following properties:
 * - Water consumption: 4 units per day (LOW requirement)
 * - Seedling duration: 2 days
 * - Growing duration: 3 days (total growth cycle: 5 days)
 * - Base price: R110.00
 * - Growth season: Year-round availability
 * 
 * Water mechanics: Each watering adds 38% to water level (approximately 4 waterings per day needed)
 */
Ivy::Ivy() : Plant("Ivy", 110.00) {
    setWaterConsumption(4);
    setSeedlingDuration(2);
    setGrowingDuration(3);
    setCharacteristics(WaterRequirement::LOW, {Season::YEAR_ROUND});
}

/**
 * @brief Waters the Ivy plant
 * 
 * Increases the plant's water level by 38%, reflecting low-moderate water requirements.
 * The water level is capped at 100% to prevent overflow.
 */
void Ivy::water() {
    int current = getWaterLevel();
    setWaterLevel(std::min(100, current + 38));  // Low-moderate water
}

/**
 * @brief Creates a deep copy of this Ivy instance
 * 
 * @return std::shared_ptr<InventoryComponent> A new Ivy instance with copied state
 * 
 * Copies all runtime state including:
 * - Unique identifier
 * - Current age
 * - Health status
 * - Current water level
 */
std::shared_ptr<InventoryComponent> Ivy::clone() const {
    auto cloned = std::make_shared<Ivy>();
    cloned->setId(getId());
    cloned->setAge(getAge());
    cloned->setHealth(getHealth());
    cloned->setWaterLevel(getWaterLevel());
    return cloned;
}

/**
 * @brief Creates a fresh blueprint instance of Ivy
 * 
 * @return std::shared_ptr<InventoryComponent> A new Ivy with default initialization
 * 
 * Used for creating new instances without copying existing state,
 * suitable for shop inventory and plant catalogs.
 */
std::shared_ptr<InventoryComponent> Ivy::blueprintClone() const { return std::make_shared<Ivy>(); }

/**
 * @brief Serializes the Ivy plant to JSON format
 * 
 * @return std::string JSON representation including type information and plant state
 * 
 * Extends base Plant serialization by prepending the specific type identifier.
 * Format: {"type":"Ivy", ...base plant data...}
 */
std::string Ivy::serialize() const {
    std::string baseJson = Plant::serialize();
    std::string result = "{\"type\":\"Ivy\",";
    result += baseJson.substr(1);
    return result;
}

/**
 * @brief Deserializes Ivy plant data from JSON format
 * 
 * @param data JSON string containing serialized plant state
 * 
 * Delegates to base Plant class for parsing all standard plant attributes.
 */
void Ivy::deserialize(const std::string& data) {
    Plant::deserialize(data);  // Base class handles all fields
}

/**
 * @brief Returns the type name of this plant
 * 
 * @return std::string The string "Ivy"
 */
std::string Ivy::typeName() const { return "Ivy"; }
