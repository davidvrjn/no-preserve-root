#include "../../include/Components/Cactus.h"

#include "../../include/Core/PlantRegistry.h"
#include "../../include/Patterns/State/PlantState.h"

/**
 * @brief Anonymous namespace for auto-registration of Cactus plant type
 * 
 * Registers the Cactus type with PlantRegistry at program startup,
 * enabling factory creation of Cactus instances.
 */
namespace {
bool registered = []() {
    PlantRegistry::registerType("Cactus", []() { return std::make_shared<Cactus>(); });
    return true;
}();
}  // namespace

/**
 * @brief Constructs a Cactus plant with default characteristics
 * 
 * Initializes a Cactus plant with the following properties:
 * - Water consumption: 1 unit per day (VERY_LOW requirement)
 * - Seedling duration: 3 days
 * - Growing duration: 4 days (total growth cycle: 7 days)
 * - Base price: R120.00
 * - Growth season: Year-round availability
 * 
 * Water mechanics: Each watering adds 18% to water level (approximately 1 watering per day needed)
 */
Cactus::Cactus() : Plant("Cactus", 120.00) {
    setWaterConsumption(1);
    setSeedlingDuration(3);
    setGrowingDuration(4);
    setCharacteristics(WaterRequirement::VERY_LOW, {Season::YEAR_ROUND});
}

/**
 * @brief Waters the Cactus plant
 * 
 * Increases the plant's water level by 18%, reflecting very low water requirements
 * typical of desert plants. The water level is capped at 100% to prevent overflow.
 */
void Cactus::water() {
    int current = getWaterLevel();
    setWaterLevel(std::min(100, current + 18));  // Very low water needs
}

/**
 * @brief Creates a deep copy of this Cactus instance
 * 
 * @return std::shared_ptr<InventoryComponent> A new Cactus instance with copied state
 * 
 * Copies all runtime state including:
 * - Unique identifier
 * - Current age
 * - Health status
 * - Current water level
 */
std::shared_ptr<InventoryComponent> Cactus::clone() const {
    auto cloned = std::make_shared<Cactus>();
    cloned->setId(getId());
    cloned->setAge(getAge());
    cloned->setHealth(getHealth());
    cloned->setWaterLevel(getWaterLevel());
    return cloned;
}

/**
 * @brief Creates a fresh blueprint instance of Cactus
 * 
 * @return std::shared_ptr<InventoryComponent> A new Cactus with default initialization
 * 
 * Used for creating new instances without copying existing state,
 * suitable for shop inventory and plant catalogs.
 */
std::shared_ptr<InventoryComponent> Cactus::blueprintClone() const {
    return std::make_shared<Cactus>();
}

/**
 * @brief Serializes the Cactus plant to JSON format
 * 
 * @return std::string JSON representation including type information and plant state
 * 
 * Extends base Plant serialization by prepending the specific type identifier.
 * The base JSON format is like: {"id":123,...}
 * Output format: {"type":"Cactus","id":123,...}
 */
std::string Cactus::serialize() const {
    // Use Plant's base serialization and add type identifier
    std::string baseJson = Plant::serialize();

    // Insert type at the beginning of the JSON object
    // baseJson is like: {"id":123,...}
    // We want: {"type":"Cactus","id":123,...}
    std::string result = "{\"type\":\"Cactus\",";
    result += baseJson.substr(1);  // Skip the opening brace from base JSON

    return result;
}

/**
 * @brief Deserializes Cactus plant data from JSON format
 * 
 * @param data JSON string containing serialized plant state
 * 
 * Delegates to base Plant class for parsing all standard plant attributes.
 */
void Cactus::deserialize(const std::string& data) {
    Plant::deserialize(data);  // Base class handles all fields
}

/**
 * @brief Returns the type name of this plant
 * 
 * @return std::string The string "Cactus"
 */
std::string Cactus::typeName() const { return "Cactus"; }
