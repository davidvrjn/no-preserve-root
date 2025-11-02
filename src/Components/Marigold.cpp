#include "../../include/Components/Marigold.h"

#include "../../include/Core/PlantRegistry.h"
#include "../../include/Patterns/State/PlantState.h"

/**
 * @brief Anonymous namespace for auto-registration of Marigold plant type
 * 
 * Registers the Marigold type with PlantRegistry at program startup,
 * enabling factory creation of Marigold instances.
 */

namespace {
bool registered = []() {
    PlantRegistry::registerType("Marigold", []() { return std::make_shared<Marigold>(); });
    return true;
}();
}  // namespace

/**
 * @brief Constructs a Marigold plant with default characteristics
 * 
 * Initializes a Marigold plant with the following properties:
 * - Water consumption: 4 units per day (LOW requirement)
 * - Seedling duration: 1 day
 * - Growing duration: 2 days (total growth cycle: 3 days)
 * - Base price: R85.00
 * - Growth season: Summer and Fall
 * 
 * Water mechanics: Each watering adds 35% to water level (approximately 4 waterings per day needed)
 */
Marigold::Marigold() : Plant("Marigold", 85.00) {
    setWaterConsumption(4);
    setSeedlingDuration(1);
    setGrowingDuration(2);
    setCharacteristics(WaterRequirement::LOW, {Season::SUMMER, Season::FALL});
}

/**
 * @brief Waters the Marigold plant
 * 
 * Increases the plant's water level by 35%, reflecting low-moderate water requirements.
 * The water level is capped at 100% to prevent overflow.
 */
void Marigold::water() {
    int current = getWaterLevel();
    setWaterLevel(std::min(100, current + 35));  // Low-moderate water
}

/**
 * @brief Creates a deep copy of this Marigold instance
 * 
 * @return std::shared_ptr<InventoryComponent> A new Marigold instance with copied state
 * 
 * Copies all runtime state including:
 * - Unique identifier
 * - Current age
 * - Health status
 * - Current water level
 */
std::shared_ptr<InventoryComponent> Marigold::clone() const {
    auto cloned = std::make_shared<Marigold>();
    cloned->setId(getId());
    cloned->setAge(getAge());
    cloned->setHealth(getHealth());
    cloned->setWaterLevel(getWaterLevel());
    return cloned;
}

/**
 * @brief Creates a fresh blueprint instance of Marigold
 * 
 * @return std::shared_ptr<InventoryComponent> A new Marigold with default initialization
 * 
 * Used for creating new instances without copying existing state,
 * suitable for shop inventory and plant catalogs.
 */

std::shared_ptr<InventoryComponent> Marigold::blueprintClone() const {
    return std::make_shared<Marigold>();
}

/**
 * @brief Serializes the Marigold plant to JSON format
 * 
 * @return std::string JSON representation including type information and plant state
 * 
 * Extends base Plant serialization by prepending the specific type identifier.
 * Format: {"type":"Marigold", ...base plant data...}
 */
std::string Marigold::serialize() const {
    std::string baseJson = Plant::serialize();
    std::string result = "{\"type\":\"Marigold\",";
    result += baseJson.substr(1);
    return result;
}

/**
 * @brief Deserializes Marigold plant data from JSON format
 * 
 * @param data JSON string containing serialized plant state
 * 
 * Delegates to base Plant class for parsing all standard plant attributes.
 */
void Marigold::deserialize(const std::string& data) {
    Plant::deserialize(data);  // Base class handles all fields
}

/**
 * @brief Returns the type name of this plant
 * 
 * @return std::string The string "Marigold"
 */
std::string Marigold::typeName() const { return "Marigold"; }
