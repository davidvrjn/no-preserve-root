#include "../../include/Components/Aloe.h"

#include "../../include/Core/PlantRegistry.h"
#include "../../include/Patterns/State/PlantState.h"

/**
 * @brief Anonymous namespace for auto-registration of Aloe plant type
 * 
 * Registers the Aloe type with PlantRegistry at program startup,
 * enabling factory creation of Aloe instances.
 */
namespace {
bool registered = []() {
    PlantRegistry::registerType("Aloe", []() { return std::make_shared<Aloe>(); });
    return true;
}();
}  // namespace

/**
 * @brief Construct a new Aloe:: Aloe object
 * 
 * Initializes an Aloe plant with the following properties:
 * - Water consumption: 2 units per day (VERY_LOW requirement)
 * - Seedling duration: 2 days
 * - Growing duration: 4 days (total growth cycle: 6 days)
 * - Base price: R110.00
 * - Growth season: Year-round availability
 * 
 * Water mechanics: Each watering adds 22% to water level (approximately 3 waterings per day needed)
 */
Aloe::Aloe() : Plant("Aloe", 110.00) {
    setWaterConsumption(2);
    setSeedlingDuration(2);
    setGrowingDuration(4);
    setCharacteristics(WaterRequirement::VERY_LOW, {Season::YEAR_ROUND});
}

/**
 * @brief Waters the Aloe plant
 * 
 * Increases the plant's water level by 22%, reflecting very low water requirements.
 * The water level is capped at 100% to prevent overflow.
 */
void Aloe::water() {
    int current = getWaterLevel();
    setWaterLevel(std::min(100, current + 22));  // Very low water needs
}

/**
 * @brief Creates a deep copy of this Aloe instance
 * 
 * @return std::shared_ptr<InventoryComponent> A new Aloe instance with copied state
 * 
 * Copies all runtime state including:
 * - Unique identifier
 * - Current age
 * - Health status
 * - Current water level
 */
std::shared_ptr<InventoryComponent> Aloe::clone() const {
    auto cloned = std::make_shared<Aloe>();
    cloned->setId(getId());
    cloned->setAge(getAge());
    cloned->setHealth(getHealth());
    cloned->setWaterLevel(getWaterLevel());
    return cloned;
}

/**
 * @brief Creates a fresh blueprint instance of Aloe
 * 
 * @return std::shared_ptr<InventoryComponent> A new Aloe with default initialization
 * 
 * Used for creating new instances without copying existing state,
 * suitable for shop inventory and plant catalogs.
 */
std::shared_ptr<InventoryComponent> Aloe::blueprintClone() const {
    return std::make_shared<Aloe>();
}

/**
 * @brief Serializes the Aloe plant to JSON format
 * 
 * @return std::string JSON representation including type information and plant state
 * 
 * Extends base Plant serialization by prepending the specific type identifier.
 * Format: {"type":"Aloe", ...base plant data...}
 */
std::string Aloe::serialize() const {
    std::string baseJson = Plant::serialize();
    std::string result = "{\"type\":\"Aloe\",";
    result += baseJson.substr(1);
    return result;
}

/**
 * @brief Deserializes Aloe plant data from JSON format
 * 
 * @param data JSON string containing serialized plant state
 * 
 * Delegates to base Plant class for parsing all standard plant attributes.
 */
void Aloe::deserialize(const std::string& data) {
    Plant::deserialize(data);  // Base class handles all fields
}

/**
 * @brief Returns the type name of this plant
 * 
 * @return std::string The string "Aloe"
 */
std::string Aloe::typeName() const { return "Aloe"; }
