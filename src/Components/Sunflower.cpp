#include "../../include/Components/Sunflower.h"

#include "../../include/Core/PlantRegistry.h"
#include "../../include/Patterns/State/PlantState.h"

/**
 * @brief Anonymous namespace for auto-registration of Sunflower plant type
 * 
 * Registers the Sunflower type with PlantRegistry at program startup,
 * enabling factory creation of Sunflower instances.
 */
namespace {
bool registered = []() {
    PlantRegistry::registerType("Sunflower", []() { return std::make_shared<Sunflower>(); });
    return true;
}();
}  // namespace

/**
 * @brief Constructs a Sunflower plant with default characteristics
 * 
 * Initializes a Sunflower plant with the following properties:
 * - Water consumption: 7 units per day (HIGH requirement)
 * - Seedling duration: 2 days
 * - Growing duration: 3 days (total growth cycle: 5 days)
 * - Base price: R120.00
 * - Growth season: Summer only
 * 
 * Water mechanics: Each watering adds 45% to water level (approximately 7 waterings per day needed)
 * Sunflowers are fast-growing, sun-loving plants with high water demands during their rapid growth.
 */
Sunflower::Sunflower() : Plant("Sunflower", 120.00) {
    setWaterConsumption(7);
    setSeedlingDuration(2);
    setGrowingDuration(3);
    setCharacteristics(WaterRequirement::HIGH, {Season::SUMMER});
}

/**
 * @brief Waters the Sunflower plant
 * 
 * Increases the plant's water level by 45%, reflecting high water requirements
 * typical of large, fast-growing flowering plants. The water level is capped at 100% to prevent overflow.
 */
void Sunflower::water() {
    int current = getWaterLevel();
    setWaterLevel(std::min(100, current + 45));  // High water needs
}

/**
 * @brief Creates a deep copy of this Sunflower instance
 * 
 * @return std::shared_ptr<InventoryComponent> A new Sunflower instance with copied state
 * 
 * Copies all runtime state including:
 * - Unique identifier
 * - Current age
 * - Health status
 * - Current water level
 */
std::shared_ptr<InventoryComponent> Sunflower::clone() const {
    auto cloned = std::make_shared<Sunflower>();
    cloned->setId(getId());
    cloned->setAge(getAge());
    cloned->setHealth(getHealth());
    cloned->setWaterLevel(getWaterLevel());
    return cloned;
}

/**
 * @brief Creates a fresh blueprint instance of Sunflower
 * 
 * @return std::shared_ptr<InventoryComponent> A new Sunflower with default initialization
 * 
 * Used for creating new instances without copying existing state,
 * suitable for shop inventory and plant catalogs.
 */
std::shared_ptr<InventoryComponent> Sunflower::blueprintClone() const {
    return std::make_shared<Sunflower>();
}

/**
 * @brief Serializes the Sunflower plant to JSON format
 * 
 * @return std::string JSON representation including type information and plant state
 * 
 * Extends base Plant serialization by prepending the specific type identifier.
 * Format: {"type":"Sunflower", ...base plant data...}
 */
std::string Sunflower::serialize() const {
    std::string baseJson = Plant::serialize();
    std::string result = "{\"type\":\"Sunflower\",";
    result += baseJson.substr(1);
    return result;
}

/**
 * @brief Deserializes Sunflower plant data from JSON format
 * 
 * @param data JSON string containing serialized plant state
 * 
 * Delegates to base Plant class for parsing all standard plant attributes.
 */
void Sunflower::deserialize(const std::string& data) {
    Plant::deserialize(data);  // Base class handles all fields
}

/**
 * @brief Returns the type name of this plant
 * 
 * @return std::string The string "Sunflower"
 */
std::string Sunflower::typeName() const { return "Sunflower"; }
