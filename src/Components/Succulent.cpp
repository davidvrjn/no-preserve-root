#include "../../include/Components/Succulent.h"

#include "../../include/Core/PlantRegistry.h"
#include "../../include/Patterns/State/PlantState.h"

/**
 * @brief Anonymous namespace for auto-registration of Succulent plant type
 * 
 * Registers the Succulent type with PlantRegistry at program startup,
 * enabling factory creation of Succulent instances.
 */
namespace {
bool registered = []() {
    PlantRegistry::registerType("Succulent", []() { return std::make_shared<Succulent>(); });
    return true;
}();
}  // namespace

/**
 * @brief Constructs a Succulent plant with default characteristics
 * 
 * Initializes a Succulent plant with the following properties:
 * - Water consumption: 2 units per day (VERY_LOW requirement)
 * - Seedling duration: 2 days
 * - Growing duration: 3 days (total growth cycle: 5 days)
 * - Base price: R95.00
 * - Growth season: Spring and Summer
 * 
 * Water mechanics: Each watering adds 25% to water level (approximately 2 waterings per day needed)
 * Succulents store water in their leaves, making them highly drought-resistant.
 */
Succulent::Succulent() : Plant("Succulent", 95.00) {
    setWaterConsumption(2);
    setSeedlingDuration(2);
    setGrowingDuration(3);
    setCharacteristics(WaterRequirement::VERY_LOW, {Season::SPRING, Season::SUMMER});
}

/**
 * @brief Waters the Succulent plant
 * 
 * Increases the plant's water level by 25%, reflecting very low water requirements.
 * The water level is capped at 100% to prevent overflow.
 */
void Succulent::water() {
    int current = getWaterLevel();
    setWaterLevel(std::min(100, current + 25));  // Very low water needs
}

/**
 * @brief Creates a deep copy of this Succulent instance
 * 
 * @return std::shared_ptr<InventoryComponent> A new Succulent instance with copied state
 * 
 * Copies all runtime state including:
 * - Unique identifier
 * - Current age
 * - Health status
 * - Current water level
 */
std::shared_ptr<InventoryComponent> Succulent::clone() const {
    auto cloned = std::make_shared<Succulent>();
    cloned->setId(getId());
    cloned->setAge(getAge());
    cloned->setHealth(getHealth());
    cloned->setWaterLevel(getWaterLevel());
    return cloned;
}

/**
 * @brief Creates a fresh blueprint instance of Succulent
 * 
 * @return std::shared_ptr<InventoryComponent> A new Succulent with default initialization
 * 
 * Used for creating new instances without copying existing state,
 * suitable for shop inventory and plant catalogs.
 */
std::shared_ptr<InventoryComponent> Succulent::blueprintClone() const {
    return std::make_shared<Succulent>();
}

/**
 * @brief Serializes the Succulent plant to JSON format
 * 
 * @return std::string JSON representation including type information and plant state
 * 
 * Extends base Plant serialization by prepending the specific type identifier.
 * Format: {"type":"Succulent", ...base plant data...}
 */
std::string Succulent::serialize() const {
    std::string baseJson = Plant::serialize();
    std::string result = "{\"type\":\"Succulent\",";
    result += baseJson.substr(1);
    return result;
}

/**
 * @brief Deserializes Succulent plant data from JSON format
 * 
 * @param data JSON string containing serialized plant state
 * 
 * Delegates to base Plant class for parsing all standard plant attributes.
 */
void Succulent::deserialize(const std::string& data) {
    Plant::deserialize(data);  // Base class handles all fields
}

/**
 * @brief Returns the type name of this plant
 * 
 * @return std::string The string "Succulent"
 */
std::string Succulent::typeName() const { return "Succulent"; }
