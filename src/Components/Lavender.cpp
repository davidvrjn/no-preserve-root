#include "../../include/Components/Lavender.h"

#include "../../include/Core/PlantRegistry.h"
#include "../../include/Patterns/State/PlantState.h"

/**
 * @brief Anonymous namespace for auto-registration of Lavender plant type
 * 
 * Registers the Lavender type with PlantRegistry at program startup,
 * enabling factory creation of Lavender instances.
 */
namespace {
bool registered = []() {
    PlantRegistry::registerType("Lavender", []() { return std::make_shared<Lavender>(); });
    return true;
}();
}  // namespace

/**
 * @brief Constructs a Lavender plant with default characteristics
 * 
 * Initializes a Lavender plant with the following properties:
 * - Water consumption: 3 units per day (LOW requirement)
 * - Seedling duration: 2 days
 * - Growing duration: 3 days (total growth cycle: 5 days)
 * - Base price: R100.00
 * - Growth season: Spring and Summer
 * 
 * Water mechanics: Each watering adds 30% to water level (approximately 3 waterings per day needed)
 */
Lavender::Lavender() : Plant("Lavender", 100.00) {
    setWaterConsumption(3);
    setSeedlingDuration(2);
    setGrowingDuration(3);
    setCharacteristics(WaterRequirement::LOW, {Season::SPRING, Season::SUMMER});
}

/**
 * @brief Waters the Lavender plant
 * 
 * Increases the plant's water level by 30%, reflecting low water requirements.
 * The water level is capped at 100% to prevent overflow.
 */
void Lavender::water() {
    int current = getWaterLevel();
    setWaterLevel(std::min(100, current + 30));  // Low water needs, adds 30
}

/**
 * @brief Creates a deep copy of this Lavender instance
 * 
 * @return std::shared_ptr<InventoryComponent> A new Lavender instance with copied state
 * 
 * Copies all runtime state including:
 * - Unique identifier
 * - Current age
 * - Health status
 * - Current water level
 */
std::shared_ptr<InventoryComponent> Lavender::clone() const {
    auto cloned = std::make_shared<Lavender>();
    cloned->setId(getId());
    cloned->setAge(getAge());
    cloned->setHealth(getHealth());
    cloned->setWaterLevel(getWaterLevel());
    return cloned;
}

/**
 * @brief Creates a fresh blueprint instance of Lavender
 * 
 * @return std::shared_ptr<InventoryComponent> A new Lavender with default initialization
 * 
 * Used for creating new instances without copying existing state,
 * suitable for shop inventory and plant catalogs.
 */
std::shared_ptr<InventoryComponent> Lavender::blueprintClone() const {
    return std::make_shared<Lavender>();
}

/**
 * @brief Serializes the Lavender plant to JSON format
 * 
 * @return std::string JSON representation including type information and plant state
 * 
 * Extends base Plant serialization by prepending the specific type identifier.
 * Format: {"type":"Lavender", ...base plant data...}
 */
std::string Lavender::serialize() const {
    std::string baseJson = Plant::serialize();
    std::string result = "{\"type\":\"Lavender\",";
    result += baseJson.substr(1);
    return result;
}

/**
 * @brief Deserializes Lavender plant data from JSON format
 * 
 * @param data JSON string containing serialized plant state (currently unused)
 * 
 * @note Current implementation does not perform deserialization.
 * Consider delegating to Plant::deserialize(data) for proper functionality.
 */
void Lavender::deserialize(const std::string& data) { (void)data; }

/**
 * @brief Returns the type name of this plant
 * 
 * @return std::string The string "Lavender"
 */
std::string Lavender::typeName() const { return "Lavender"; }
