#include "../../include/Components/Fern.h"

#include "../../include/Core/PlantRegistry.h"
#include "../../include/Patterns/State/PlantState.h"

/**
 * @brief Anonymous namespace for auto-registration of Fern plant type
 * 
 * Registers the Fern type with PlantRegistry at program startup,
 * enabling factory creation of Fern instances.
 */
namespace {
bool registered = []() {
    PlantRegistry::registerType("Fern", []() { return std::make_shared<Fern>(); });
    return true;
}();
}  // namespace

/**
 * @brief Constructs a Fern plant with default characteristics
 * 
 * Initializes a Fern plant with the following properties:
 * - Water consumption: 8 units per day (HIGH requirement)
 * - Seedling duration: 2 days
 * - Growing duration: 4 days (total growth cycle: 6 days)
 * - Base price: R130.00
 * - Growth season: Year-round availability
 * 
 * Water mechanics: Each watering adds 50% to water level (approximately 8 waterings per day needed)
 */
Fern::Fern() : Plant("Fern", 130.00) {
    setWaterConsumption(8);
    setSeedlingDuration(2);
    setGrowingDuration(4);
    setCharacteristics(WaterRequirement::HIGH, {Season::YEAR_ROUND});
}

/**
 * @brief Waters the Fern plant
 * 
 * Increases the plant's water level by 50%, reflecting high water requirements
 * typical of moisture-loving plants. The water level is capped at 100% to prevent overflow.
 */
void Fern::water() {
    int current = getWaterLevel();
    setWaterLevel(std::min(100, current + 50));  // High water needs
}

/**
 * @brief Creates a deep copy of this Fern instance
 * 
 * @return std::shared_ptr<InventoryComponent> A new Fern instance with copied state
 * 
 * Copies all runtime state including:
 * - Unique identifier
 * - Current age
 * - Health status
 * - Current water level
 */
std::shared_ptr<InventoryComponent> Fern::clone() const {
    auto cloned = std::make_shared<Fern>();
    cloned->setId(getId());
    cloned->setAge(getAge());
    cloned->setHealth(getHealth());
    cloned->setWaterLevel(getWaterLevel());
    return cloned;
}

/**
 * @brief Creates a fresh blueprint instance of Fern
 * 
 * @return std::shared_ptr<InventoryComponent> A new Fern with default initialization
 * 
 * Used for creating new instances without copying existing state,
 * suitable for shop inventory and plant catalogs.
 */
std::shared_ptr<InventoryComponent> Fern::blueprintClone() const {
    return std::make_shared<Fern>();
}

/**
 * @brief Serializes the Fern plant to JSON format
 * 
 * @return std::string JSON representation including type information and plant state
 * 
 * Extends base Plant serialization by prepending the specific type identifier.
 * Format: {"type":"Fern", ...base plant data...}
 */
std::string Fern::serialize() const {
    std::string baseJson = Plant::serialize();
    std::string result = "{\"type\":\"Fern\",";
    result += baseJson.substr(1);
    return result;
}

/**
 * @brief Deserializes Fern plant data from JSON format
 * 
 * @param data JSON string containing serialized plant state
 * 
 * Delegates to base Plant class for parsing all standard plant attributes.
 */
void Fern::deserialize(const std::string& data) {
    Plant::deserialize(data);  // Base class handles all fields
}

/**
 * @brief Returns the type name of this plant
 * 
 * @return std::string The string "Fern"
 */
std::string Fern::typeName() const { return "Fern"; }
