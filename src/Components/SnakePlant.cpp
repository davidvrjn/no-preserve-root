#include "../../include/Components/SnakePlant.h"

#include "../../include/Core/PlantRegistry.h"
#include "../../include/Patterns/State/PlantState.h"

/**
 * @brief Anonymous namespace for auto-registration of SnakePlant plant type
 * 
 * Registers the SnakePlant type with PlantRegistry at program startup,
 * enabling factory creation of SnakePlant instances.
 */
namespace {
bool registered = []() {
    PlantRegistry::registerType("SnakePlant", []() { return std::make_shared<SnakePlant>(); });
    return true;
}();
}  // namespace

/**
 * @brief Constructs a SnakePlant with default characteristics
 * 
 * Initializes a SnakePlant with the following properties:
 * - Water consumption: 1 unit per day (VERY_LOW requirement)
 * - Seedling duration: 3 days
 * - Growing duration: 4 days (total growth cycle: 7 days)
 * - Base price: R120.00
 * - Growth season: Year-round availability
 * 
 * Water mechanics: Each watering adds 20% to water level (approximately 1 watering per day needed)
 * Snake plants are extremely drought-tolerant and thrive with minimal care.
 */
SnakePlant::SnakePlant() : Plant("SnakePlant", 120.00) {
    setWaterConsumption(1);
    setSeedlingDuration(3);
    setGrowingDuration(4);
    setCharacteristics(WaterRequirement::VERY_LOW, {Season::YEAR_ROUND});
}

/**
 * @brief Waters the SnakePlant
 * 
 * Increases the plant's water level by 20%, reflecting very low water requirements
 * typical of hardy, drought-tolerant plants. The water level is capped at 100% to prevent overflow.
 */
void SnakePlant::water() {
    int current = getWaterLevel();
    setWaterLevel(std::min(100, current + 20));  // Very low water needs
}

/**
 * @brief Creates a deep copy of this SnakePlant instance
 * 
 * @return std::shared_ptr<InventoryComponent> A new SnakePlant instance with copied state
 * 
 * Copies all runtime state including:
 * - Unique identifier
 * - Current age
 * - Health status
 * - Current water level
 */
std::shared_ptr<InventoryComponent> SnakePlant::clone() const {
    auto cloned = std::make_shared<SnakePlant>();
    cloned->setId(getId());
    cloned->setAge(getAge());
    cloned->setHealth(getHealth());
    cloned->setWaterLevel(getWaterLevel());
    return cloned;
}

/**
 * @brief Creates a fresh blueprint instance of SnakePlant
 * 
 * @return std::shared_ptr<InventoryComponent> A new SnakePlant with default initialization
 * 
 * Used for creating new instances without copying existing state,
 * suitable for shop inventory and plant catalogs.
 */
std::shared_ptr<InventoryComponent> SnakePlant::blueprintClone() const {
    return std::make_shared<SnakePlant>();
}

/**
 * @brief Serializes the SnakePlant to JSON format
 * 
 * @return std::string JSON representation including type information and plant state
 * 
 * Extends base Plant serialization by prepending the specific type identifier.
 * Format: {"type":"SnakePlant", ...base plant data...}
 */
std::string SnakePlant::serialize() const {
    std::string baseJson = Plant::serialize();
    std::string result = "{\"type\":\"SnakePlant\",";
    result += baseJson.substr(1);
    return result;
}

/**
 * @brief Deserializes SnakePlant data from JSON format
 * 
 * @param data JSON string containing serialized plant state
 * 
 * Delegates to base Plant class for parsing all standard plant attributes.
 */
void SnakePlant::deserialize(const std::string& data) {
    Plant::deserialize(data);  // Base class handles all fields
}

/**
 * @brief Returns the type name of this plant
 * 
 * @return std::string The string "SnakePlant"
 */
std::string SnakePlant::typeName() const { return "SnakePlant"; }
