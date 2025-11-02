#include "../../include/Components/Orchid.h"

#include "../../include/Core/PlantRegistry.h"
#include "../../include/Patterns/State/PlantState.h"

/**
 * @brief Anonymous namespace for auto-registration of Orchid plant type
 * 
 * Registers the Orchid type with PlantRegistry at program startup,
 * enabling factory creation of Orchid instances.
 */
namespace {
bool registered = []() {
    PlantRegistry::registerType("Orchid", []() { return std::make_shared<Orchid>(); });
    return true;
}();
}  // namespace

/**
 * @brief Constructs an Orchid plant with default characteristics
 * 
 * Initializes an Orchid plant with the following properties:
 * - Water consumption: 4 units per day (LOW requirement)
 * - Seedling duration: 3 days
 * - Growing duration: 4 days (total growth cycle: 7 days)
 * - Base price: R145.00
 * - Growth season: Year-round availability
 * 
 * Water mechanics: Each watering adds 35% to water level (approximately 4 waterings per day needed)
 * Orchids require careful, moderate watering despite low consumption rating.
 */
Orchid::Orchid() : Plant("Orchid", 145.00) {
    setWaterConsumption(4);
    setSeedlingDuration(3);
    setGrowingDuration(4);
    setCharacteristics(WaterRequirement::LOW, {Season::YEAR_ROUND});
}

/**
 * @brief Waters the Orchid plant
 * 
 * Increases the plant's water level by 35%, reflecting the careful, moderate watering
 * needs of orchids. Despite being classified as LOW water requirement, orchids benefit
 * from consistent moisture. The water level is capped at 100% to prevent overflow.
 */

void Orchid::water() {
    int current = getWaterLevel();
    setWaterLevel(std::min(100, current + 35));  // Moderate, careful watering
}

/**
 * @brief Creates a deep copy of this Orchid instance
 * 
 * @return std::shared_ptr<InventoryComponent> A new Orchid instance with copied state
 * 
 * Copies all runtime state including:
 * - Unique identifier
 * - Current age
 * - Health status
 * - Current water level
 */
std::shared_ptr<InventoryComponent> Orchid::clone() const {
    auto cloned = std::make_shared<Orchid>();
    cloned->setId(getId());
    cloned->setAge(getAge());
    cloned->setHealth(getHealth());
    cloned->setWaterLevel(getWaterLevel());
    return cloned;
}

/**
 * @brief Creates a fresh blueprint instance of Orchid
 * 
 * @return std::shared_ptr<InventoryComponent> A new Orchid with default initialization
 * 
 * Used for creating new instances without copying existing state,
 * suitable for shop inventory and plant catalogs.
 */
std::shared_ptr<InventoryComponent> Orchid::blueprintClone() const {
    return std::make_shared<Orchid>();
}

/**
 * @brief Serializes the Orchid plant to JSON format
 * 
 * @return std::string JSON representation including type information and plant state
 * 
 * Extends base Plant serialization by prepending the specific type identifier.
 * Format: {"type":"Orchid", ...base plant data...}
 */
std::string Orchid::serialize() const {
    std::string baseJson = Plant::serialize();
    std::string result = "{\"type\":\"Orchid\",";
    result += baseJson.substr(1);
    return result;
}

/**
 * @brief Deserializes Orchid plant data from JSON format
 * 
 * @param data JSON string containing serialized plant state
 * 
 * Delegates to base Plant class for parsing all standard plant attributes.
 */
void Orchid::deserialize(const std::string& data) {
    Plant::deserialize(data);  // Base class handles all fields
}

/**
 * @brief Returns the type name of this plant
 * 
 * @return std::string The string "Orchid"
 */
std::string Orchid::typeName() const { return "Orchid"; }
