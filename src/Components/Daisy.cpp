#include "../../include/Components/Daisy.h"

#include "../../include/Core/PlantRegistry.h"
#include "../../include/Patterns/State/PlantState.h"

/**
 * @brief Anonymous namespace for auto-registration of Daisy plant type
 * 
 * Registers the Daisy type with PlantRegistry at program startup,
 * enabling factory creation of Daisy instances.
 */
namespace {
bool registered = []() {
    PlantRegistry::registerType("Daisy", []() { return std::make_shared<Daisy>(); });
    return true;
}();
}  // namespace

/**
 * @brief Constructs a Daisy plant with default characteristics
 * 
 * Initializes a Daisy plant with the following properties:
 * - Water consumption: 5 units per day (MEDIUM requirement)
 * - Seedling duration: 1 day
 * - Growing duration: 3 days (total growth cycle: 4 days)
 * - Base price: R100.00
 * - Growth season: Spring and Summer
 * 
 * Water mechanics: Each watering adds 40% to water level (approximately 5 waterings per day needed)
 */
Daisy::Daisy() : Plant("Daisy", 100.00) {
    setWaterConsumption(5);
    setSeedlingDuration(1);
    setGrowingDuration(3);
    setCharacteristics(WaterRequirement::MEDIUM, {Season::SPRING, Season::SUMMER});
}

/**
 * @brief Waters the Daisy plant
 * 
 * Increases the plant's water level by 40%, reflecting moderate water requirements.
 * The water level is capped at 100% to prevent overflow.
 */
void Daisy::water() {
    int current = getWaterLevel();
    setWaterLevel(std::min(100, current + 40));  // Moderate water needs
}

/**
 * @brief Creates a deep copy of this Daisy instance
 * 
 * @return std::shared_ptr<InventoryComponent> A new Daisy instance with copied state
 * 
 * Copies all runtime state including:
 * - Unique identifier
 * - Current age
 * - Health status
 * - Current water level
 */
std::shared_ptr<InventoryComponent> Daisy::clone() const {
    auto cloned = std::make_shared<Daisy>();
    cloned->setId(getId());
    cloned->setAge(getAge());
    cloned->setHealth(getHealth());
    cloned->setWaterLevel(getWaterLevel());
    return cloned;
}

/**
 * @brief Creates a fresh blueprint instance of Daisy
 * 
 * @return std::shared_ptr<InventoryComponent> A new Daisy with default initialization
 * 
 * Used for creating new instances without copying existing state,
 * suitable for shop inventory and plant catalogs.
 */
std::shared_ptr<InventoryComponent> Daisy::blueprintClone() const {
    return std::make_shared<Daisy>();
}

/**
 * @brief Serializes the Daisy plant to JSON format
 * 
 * @return std::string JSON representation including type information and plant state
 * 
 * Extends base Plant serialization by prepending the specific type identifier.
 * Format: {"type":"Daisy", ...base plant data...}
 */
std::string Daisy::serialize() const {
    std::string baseJson = Plant::serialize();
    std::string result = "{\"type\":\"Daisy\",";
    result += baseJson.substr(1);
    return result;
}

/**
 * @brief Deserializes Daisy plant data from JSON format
 * 
 * @param data JSON string containing serialized plant state
 * 
 * Delegates to base Plant class for parsing all standard plant attributes.
 */
void Daisy::deserialize(const std::string& data) {
    Plant::deserialize(data);  // Base class handles all fields
}

/**
 * @brief Returns the type name of this plant
 * 
 * @return std::string The string "Daisy"
 */
std::string Daisy::typeName() const { return "Daisy"; }
