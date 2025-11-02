#include "../../include/Components/Mint.h"

#include "../../include/Core/PlantRegistry.h"
#include "../../include/Patterns/State/PlantState.h"

/**
 * @brief Anonymous namespace for auto-registration of Mint plant type
 * 
 * Registers the Mint type with PlantRegistry at program startup,
 * enabling factory creation of Mint instances.
 */
namespace {
bool registered = []() {
    PlantRegistry::registerType("Mint", []() { return std::make_shared<Mint>(); });
    return true;
}();
}  // namespace

/**
 * @brief Constructs a Mint plant with default characteristics
 * 
 * Initializes a Mint plant with the following properties:
 * - Water consumption: 7 units per day (HIGH requirement)
 * - Seedling duration: 1 day
 * - Growing duration: 2 days (total growth cycle: 3 days)
 * - Base price: R105.00
 * - Growth season: Spring, Summer, and Fall
 * 
 * Water mechanics: Each watering adds 48% to water level (approximately 7 waterings per day needed)
 */
Mint::Mint() : Plant("Mint", 105.00) {
    setWaterConsumption(7);
    setSeedlingDuration(1);
    setGrowingDuration(2);
    setCharacteristics(WaterRequirement::HIGH, {Season::SPRING, Season::SUMMER, Season::FALL});
}

/**
 * @brief Waters the Mint plant
 * 
 * Increases the plant's water level by 48%, reflecting high water requirements
 * typical of moisture-loving herbs. The water level is capped at 100% to prevent overflow.
 */
void Mint::water() {
    int current = getWaterLevel();
    setWaterLevel(std::min(100, current + 48));  // High water needs
}

/**
 * @brief Creates a deep copy of this Mint instance
 * 
 * @return std::shared_ptr<InventoryComponent> A new Mint instance with copied state
 * 
 * Copies all runtime state including:
 * - Unique identifier
 * - Current age
 * - Health status
 * - Current water level
 */
std::shared_ptr<InventoryComponent> Mint::clone() const {
    auto cloned = std::make_shared<Mint>();
    cloned->setId(getId());
    cloned->setAge(getAge());
    cloned->setHealth(getHealth());
    cloned->setWaterLevel(getWaterLevel());
    return cloned;
}

/**
 * @brief Creates a fresh blueprint instance of Mint
 * 
 * @return std::shared_ptr<InventoryComponent> A new Mint with default initialization
 * 
 * Used for creating new instances without copying existing state,
 * suitable for shop inventory and plant catalogs.
 */
std::shared_ptr<InventoryComponent> Mint::blueprintClone() const {
    return std::make_shared<Mint>();
}

/**
 * @brief Serializes the Mint plant to JSON format
 * 
 * @return std::string JSON representation including type information and plant state
 * 
 * Extends base Plant serialization by prepending the specific type identifier.
 * Format: {"type":"Mint", ...base plant data...}
 */
std::string Mint::serialize() const {
    std::string baseJson = Plant::serialize();
    std::string result = "{\"type\":\"Mint\",";
    result += baseJson.substr(1);
    return result;
}

/**
 * @brief Deserializes Mint plant data from JSON format
 * 
 * @param data JSON string containing serialized plant state
 * 
 * Delegates to base Plant class for parsing all standard plant attributes.
 */
void Mint::deserialize(const std::string& data) {
    Plant::deserialize(data);  // Base class handles all fields
}

/**
 * @brief Returns the type name of this plant
 * 
 * @return std::string The string "Mint"
 */
std::string Mint::typeName() const { return "Mint"; }
