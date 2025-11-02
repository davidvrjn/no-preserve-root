#include "../../include/Components/Rose.h"

#include "../../include/Core/PlantRegistry.h"
#include "../../include/Patterns/State/PlantState.h"

/**
 * @brief Anonymous namespace for auto-registration of Rose plant type
 * 
 * Registers the Rose type with PlantRegistry at program startup,
 * enabling factory creation of Rose instances.
 */
namespace {
bool registered = []() {
    PlantRegistry::registerType("Rose", []() { return std::make_shared<Rose>(); });
    return true;
}();
}  // namespace

/**
 * @brief Constructs a Rose plant with default characteristics
 * 
 * Initializes a Rose plant with the following properties:
 * - Water consumption: 6 units per day (MEDIUM requirement)
 * - Seedling duration: 2 days
 * - Growing duration: 4 days (total growth cycle: 6 days)
 * - Base price: R135.00
 * - Growth season: Spring, Summer, and Fall
 * 
 * Water mechanics: Each watering adds 42% to water level (approximately 6 waterings per day needed)
 */
Rose::Rose() : Plant("Rose", 135.00) {
    setWaterConsumption(6);
    setSeedlingDuration(2);
    setGrowingDuration(4);
    setCharacteristics(WaterRequirement::MEDIUM, {Season::SPRING, Season::SUMMER, Season::FALL});
}

/**
 * @brief Waters the Rose plant
 * 
 * Increases the plant's water level by 42%, reflecting moderate-high water requirements.
 * The water level is capped at 100% to prevent overflow.
 */
void Rose::water() {
    int current = getWaterLevel();
    setWaterLevel(std::min(100, current + 42));  // Moderate-high water needs
}

/**
 * @brief Creates a deep copy of this Rose instance
 * 
 * @return std::shared_ptr<InventoryComponent> A new Rose instance with copied state
 * 
 * Copies all runtime state including:
 * - Unique identifier
 * - Current age
 * - Health status
 * - Current water level
 */
std::shared_ptr<InventoryComponent> Rose::clone() const {
    auto cloned = std::make_shared<Rose>();
    cloned->setId(getId());
    cloned->setAge(getAge());
    cloned->setHealth(getHealth());
    cloned->setWaterLevel(getWaterLevel());
    return cloned;
}

/**
 * @brief Creates a fresh blueprint instance of Rose
 * 
 * @return std::shared_ptr<InventoryComponent> A new Rose with default initialization
 * 
 * Used for creating new instances without copying existing state,
 * suitable for shop inventory and plant catalogs.
 */
std::shared_ptr<InventoryComponent> Rose::blueprintClone() const {
    return std::make_shared<Rose>();
}

/**
 * @brief Serializes the Rose plant to JSON format
 * 
 * @return std::string JSON representation including type information and plant state
 * 
 * Extends base Plant serialization by prepending the specific type identifier.
 * The base JSON format is like: {"id":123,...}
 * Output format: {"type":"Rose","id":123,...}
 */
std::string Rose::serialize() const {
    // Use Plant's base serialization and add type identifier
    std::string baseJson = Plant::serialize();

    // Insert type at the beginning of the JSON object
    // baseJson is like: {"id":123,...}
    // We want: {"type":"Rose","id":123,...}
    std::string result = "{\"type\":\"Rose\",";
    result += baseJson.substr(1);  // Skip the opening brace from base JSON

    return result;
}

/**
 * @brief Deserializes Rose plant data from JSON format
 * 
 * @param data JSON string containing serialized plant state
 * 
 * Delegates to base Plant class for parsing all standard plant attributes.
 */
void Rose::deserialize(const std::string& data) {
    Plant::deserialize(data);  // Base class handles all fields
}

/**
 * @brief Returns the type name of this plant
 * 
 * @return std::string The string "Rose"
 */
std::string Rose::typeName() const { return "Rose"; }
