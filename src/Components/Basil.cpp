#include "../../include/Components/Basil.h"

#include "../../include/Core/PlantRegistry.h"
#include "../../include/Patterns/State/PlantState.h"

/**
 * @brief Anonymous namespace for auto-registration of Basil plant type
 * 
 * Auto-Registers the Basil type with PlantRegistry at program startup,
 * enabling factory creation of Basil instances.
 */
namespace {
bool registered = []() {
    PlantRegistry::registerType("Basil", []() { return std::make_shared<Basil>(); });
    return true;
}();
}  // namespace

/**
 * @brief Constructs a Basil plant with default characteristics
 * 
 * Initializes a Basil plant with the following properties:
 * - Water consumption: 6 units per day (MEDIUM requirement)
 * - Seedling duration: 1 day
 * - Growing duration: 2 days (total growth cycle: 3 days)
 * - Base price: R90.00
 * - Growth season: Summer only
 * 
 * Water mechanics: Each watering adds 43% to water level (approximately 6 waterings per day needed)
 */
Basil::Basil() : Plant("Basil", 90.00) {
    setWaterConsumption(6);
    setSeedlingDuration(1);
    setGrowingDuration(2);
    setCharacteristics(WaterRequirement::MEDIUM, {Season::SUMMER});
}

/**
 * @brief Waters the Basil plant
 * 
 * Increases the plant's water level by 43%, reflecting moderate-high water requirements.
 * The water level is capped at 100% to prevent overflow.
 */
void Basil::water() {
    int current = getWaterLevel();
    setWaterLevel(std::min(100, current + 43));  // Moderate-high water
}

/**
 * @brief Creates a deep copy of this Basil instance
 * 
 * @return std::shared_ptr<InventoryComponent> A new Basil instance with copied state
 * 
 * Copies all runtime state including:
 * - Unique identifier
 * - Current age
 * - Health status
 * - Current water level
 */
std::shared_ptr<InventoryComponent> Basil::clone() const {
    auto cloned = std::make_shared<Basil>();
    cloned->setId(getId());
    cloned->setAge(getAge());
    cloned->setHealth(getHealth());
    cloned->setWaterLevel(getWaterLevel());
    return cloned;
}

/**
 * @brief Creates a fresh blueprint instance of Basil
 * 
 * @return std::shared_ptr<InventoryComponent> A new Basil with default initialization
 * 
 * Used for creating new instances without copying existing state,
 * suitable for shop inventory and plant catalogs.
 */
std::shared_ptr<InventoryComponent> Basil::blueprintClone() const {
    return std::make_shared<Basil>();
}

/**
 * @brief Serializes the Basil plant to JSON format
 * 
 * @return std::string JSON representation including type information and plant state
 * 
 * Extends base Plant serialization by prepending the specific type identifier.
 * Format: {"type":"Basil", ...base plant data...}
 */
std::string Basil::serialize() const {
    std::string baseJson = Plant::serialize();
    std::string result = "{\"type\":\"Basil\",";
    result += baseJson.substr(1);
    return result;
}

/**
 * @brief Deserializes Basil plant data from JSON format
 * 
 * @param data JSON string containing serialized plant state
 * 
 * Delegates to base Plant class for parsing all standard plant attributes.
 */
void Basil::deserialize(const std::string& data) {
    Plant::deserialize(data);  // Base class handles all fields
}

/**
 * @brief Returns the type name of this plant
 * 
 * @return std::string The string "Basil"
 */
std::string Basil::typeName() const { return "Basil"; }
