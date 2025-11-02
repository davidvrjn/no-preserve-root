#include "../../include/Components/Tulip.h"

#include "../../include/Core/PlantRegistry.h"
#include "../../include/Patterns/State/PlantState.h"

/**
 * @brief Anonymous namespace for auto-registration of Tulip plant type
 * 
 * Registers the Tulip type with PlantRegistry at program startup,
 * enabling factory creation of Tulip instances.
 */
namespace {
bool registered = []() {
    PlantRegistry::registerType("Tulip", []() { return std::make_shared<Tulip>(); });
    return true;
}();
}  // namespace

/**
 * @brief Constructs a Tulip plant with default characteristics
 * 
 * Initializes a Tulip plant with the following properties:
 * - Water consumption: 5 units per day (MEDIUM requirement)
 * - Seedling duration: 2 days
 * - Growing duration: 3 days (total growth cycle: 5 days)
 * - Base price: R105.00
 * - Growth season: Spring only
 * 
 * Water mechanics: Each watering adds 40% to water level (approximately 5 waterings per day needed)
 * Tulips are classic spring flowers with moderate water requirements and a relatively fast growth cycle.
 */
Tulip::Tulip() : Plant("Tulip", 105.00) {
    setWaterConsumption(5);
    setSeedlingDuration(2);
    setGrowingDuration(3);
    setCharacteristics(WaterRequirement::MEDIUM, {Season::SPRING});
}

/**
 * @brief Waters the Tulip plant
 * 
 * Increases the plant's water level by 40%, reflecting moderate water requirements.
 * The water level is capped at 100% to prevent overflow.
 */
void Tulip::water() {
    int current = getWaterLevel();
    setWaterLevel(std::min(100, current + 40));  // Moderate water needs
}

/**
 * @brief Creates a deep copy of this Tulip instance
 * 
 * @return std::shared_ptr<InventoryComponent> A new Tulip instance with copied state
 * 
 * Copies all runtime state including:
 * - Unique identifier
 * - Current age
 * - Health status
 * - Current water level
 */
std::shared_ptr<InventoryComponent> Tulip::clone() const {
    auto cloned = std::make_shared<Tulip>();
    cloned->setId(getId());
    cloned->setAge(getAge());
    cloned->setHealth(getHealth());
    cloned->setWaterLevel(getWaterLevel());
    return cloned;
}

/**
 * @brief Creates a fresh blueprint instance of Tulip
 * 
 * @return std::shared_ptr<InventoryComponent> A new Tulip with default initialization
 * 
 * Used for creating new instances without copying existing state,
 * suitable for shop inventory and plant catalogs.
 */
std::shared_ptr<InventoryComponent> Tulip::blueprintClone() const {
    return std::make_shared<Tulip>();
}

/**
 * @brief Serializes the Tulip plant to JSON format
 * 
 * @return std::string JSON representation including type information and plant state
 * 
 * Extends base Plant serialization by prepending the specific type identifier.
 * Format: {"type":"Tulip", ...base plant data...}
 */
std::string Tulip::serialize() const {
    std::string baseJson = Plant::serialize();
    std::string result = "{\"type\":\"Tulip\",";
    result += baseJson.substr(1);
    return result;
}

/**
 * @brief Deserializes Tulip plant data from JSON format
 * 
 * @param data JSON string containing serialized plant state
 * 
 * Delegates to base Plant class for parsing all standard plant attributes.
 */
void Tulip::deserialize(const std::string& data) {
    Plant::deserialize(data);  // Base class handles all fields
}

/**
 * @brief Returns the type name of this plant
 * 
 * @return std::string The string "Tulip"
 */
std::string Tulip::typeName() const { return "Tulip"; }
