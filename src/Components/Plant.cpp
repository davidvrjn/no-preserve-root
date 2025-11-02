#include "../../include/Components/Plant.h"

#include <algorithm>
#include <sstream>

#include "../../include/Components/PlantAttributes.h"
#include "../../include/Core/DeserializationUtils.h"
#include "../../include/Patterns/Iterator/Iterator.h"
#include "../../include/Patterns/Observer/Observer.h"
#include "../../include/Patterns/State/Growing.h"
#include "../../include/Patterns/State/Mature.h"
#include "../../include/Patterns/State/PlantState.h"
#include "../../include/Patterns/State/Seedling.h"
#include "../../include/Patterns/State/Withered.h"
#include "../../include/Patterns/State/Withering.h"
#include "../../include/json.hpp"
/**
 * @brief Construct a new Plant:: Plant object
 * 
 * @param name The display name of the plant
 * @param price The base selling price of the plant in currency units
 * 
 * Initializes a plant with default values:
 * - Age: 0 days
 * - Health: 100%
 * - Water level: 100%
 * - Water consumption: 5 units per day (MEDIUM)
 * - Seedling duration: 10 days
 * - Growing duration: 20 days
 * - Water requirement: MEDIUM
 * - Preferred seasons: YEAR_ROUND (default, typically overridden by subclasses)
 * - Current state: nullptr (must be set after construction)
 */
Plant::Plant(const std::string& name, double price)
    : name(name),
      price(price),
      age(0),
      health(100),
      waterLevel(100),
      waterConsumption(5),
      seedlingDuration(10),
      growingDuration(20),
      waterRequirement(WaterRequirement::MEDIUM),
      currentState(nullptr) {
    // Default to year-round if not specified by subclass
    preferredSeasons.push_back(Season::YEAR_ROUND);
}

/**
 * @brief Virtual destructor for proper cleanup of derived classes
 */
Plant::~Plant() = default;

/**
 * @brief Gets the display name of the plant
 * 
 * @return std::string The plant's name (e.g., "Rose", "Cactus")
 */
std::string Plant::getName() const { return name; }

/**
 * @brief Gets the base price of the plant
 * 
 * @return double The base price before seasonal adjustments
 * 
 * @note Use getSeasonalPrice() to get the price adjusted for current season
 */
double Plant::getPrice() const { return price; }

/**
 * @brief Creates an iterator for a leaf node (Plant)
 * @return A simple iterator that returns only this plant, then stops
 *
 * Plants are leaf nodes in the composite structure, so they have no children
 * to traverse. The iterator returns the plant itself once, then finishes.
 */
std::unique_ptr<Iterator> Plant::createIterator() {
    // For leaf nodes, create a simple iterator that returns just this component
    class LeafIterator : public Iterator {
       private:
        std::shared_ptr<InventoryComponent> component;
        bool consumed = false;

       public:
        explicit LeafIterator(std::shared_ptr<InventoryComponent> comp)
            : component(std::move(comp)) {}

        std::shared_ptr<InventoryComponent> next() override {
            if (!consumed && component) {
                consumed = true;
                return component;
            }
            return nullptr;
        }

        bool hasNext() const override { return !consumed && component != nullptr; }
    };

    // Get shared_ptr to this plant through Subject base, then cast to InventoryComponent
    auto subjectPtr = Subject::shared_from_this();
    auto inventoryPtr = std::dynamic_pointer_cast<InventoryComponent>(subjectPtr);
    return std::make_unique<LeafIterator>(inventoryPtr);
}

/**
 * @brief Clone method for Plant base class
 *
 * Note: Plant is abstract (has pure virtual water()), so it cannot be instantiated directly.
 * This method should never be called on a pure Plant* - always called on concrete subclasses
 * (Rose, Cactus, etc.) which override this method.
 *
 * Returning nullptr here as a safeguard. In practice, this should not be reached because:
 * 1. Plant cannot be instantiated (abstract class)
 * 2. All concrete plants override clone()
 *
 * @return nullptr (should never be called)
 */
std::shared_ptr<InventoryComponent> Plant::clone() const {
    // Plant is abstract - clone should be called on concrete subclasses
    return nullptr;
}

/**
 * @brief Blueprint clone for Plant base class
 *
 * Same rationale as clone() - should never be called on abstract Plant class.
 * Concrete plant subclasses override this method.
 *
 * @return nullptr (should never be called)
 */
std::shared_ptr<InventoryComponent> Plant::blueprintClone() const {
    // Plant is abstract - blueprintClone should be called on concrete subclasses
    return nullptr;
}

/**
 * @brief Serializes the plant to JSON format
 * 
 * @return std::string JSON representation of the plant's complete state
 * 
 * Serializes all plant properties including:
 * - Identity: id, name, type
 * - Economics: price
 * - State: age, health, waterLevel
 * - Configuration: waterConsumption, seedlingDuration, growingDuration
 * - Characteristics: waterRequirement, preferredSeasons
 * - Current state: polymorphic state object (Seedling, Growing, Mature, Withering, Withered)
 * 
 * Special handling for Withering state which includes previousStateType information.
 * 
 * Format example:
 * {
 *   "id": 123,
 *   "name": "Rose",
 *   "price": 135.00,
 *   "age": 5,
 *   "health": 80,
 *   "waterLevel": 50,
 *   "waterConsumption": 6,
 *   "seedlingDuration": 2,
 *   "growingDuration": 4,
 *   "waterRequirement": "MEDIUM",
 *   "preferredSeasons": ["SPRING", "SUMMER", "FALL"],
 *   "state": {"type": "Growing"}
 * }
 */
std::string Plant::serialize() const {
    std::ostringstream json;
    json << "{";

    // Plant base properties
    json << "\"id\":" << getId() << ",";
    json << "\"name\":\"" << name << "\",";
    json << "\"price\":" << price << ",";
    json << "\"age\":" << age << ",";
    json << "\"health\":" << health << ",";
    json << "\"waterLevel\":" << waterLevel << ",";
    json << "\"waterConsumption\":" << waterConsumption << ",";
    json << "\"seedlingDuration\":" << seedlingDuration << ",";
    json << "\"growingDuration\":" << growingDuration << ",";

    // WaterRequirement enum
    json << "\"waterRequirement\":\"";
    switch (waterRequirement) {
        case WaterRequirement::VERY_LOW:
            json << "VERY_LOW";
            break;
        case WaterRequirement::LOW:
            json << "LOW";
            break;
        case WaterRequirement::MEDIUM:
            json << "MEDIUM";
            break;
        case WaterRequirement::HIGH:
            json << "HIGH";
            break;
    }
    json << "\",";

    // Preferred seasons array
    json << "\"preferredSeasons\":[";
    for (size_t i = 0; i < preferredSeasons.size(); ++i) {
        if (i > 0) json << ",";
        json << "\"";
        switch (preferredSeasons[i]) {
            case Season::SPRING:
                json << "SPRING";
                break;
            case Season::SUMMER:
                json << "SUMMER";
                break;
            case Season::FALL:
                json << "FALL";
                break;
            case Season::WINTER:
                json << "WINTER";
                break;
            case Season::YEAR_ROUND:
                json << "YEAR_ROUND";
                break;
        }
        json << "\"";
    }
    json << "],";

    // Serialize state (polymorphic - need type identification)
    json << "\"state\":";

    if (!currentState) {
        // No state set yet (null)
        json << "null";
    } else {
        json << "{";
        json << "\"type\":\"";

        // Determine state type using dynamic_cast
        if (dynamic_cast<Seedling*>(currentState.get())) {
            json << "Seedling\"";
        } else if (dynamic_cast<Growing*>(currentState.get())) {
            json << "Growing\"";
        } else if (dynamic_cast<Mature*>(currentState.get())) {
            json << "Mature\"";
        } else if (auto* witheringState = dynamic_cast<Withering*>(currentState.get())) {
            json << "Withering\"";
            // Withering has previousState that needs serialization
            json << ",\"previousStateType\":\"";

            // Get previous state and determine its type
            const PlantState* prevState = witheringState->getPreviousState();
            if (prevState) {
                if (dynamic_cast<const Seedling*>(prevState)) {
                    json << "Seedling";
                } else if (dynamic_cast<const Growing*>(prevState)) {
                    json << "Growing";
                } else if (dynamic_cast<const Mature*>(prevState)) {
                    json << "Mature";
                } else {
                    json << "Unknown";
                }
            } else {
                json << "None";
            }
            json << "\"";
        } else if (dynamic_cast<Withered*>(currentState.get())) {
            json << "Withered\"";
        } else {
            json << "Unknown\"";
        }

        json << "}";
    }

    json << "}";
    return json.str();
}

/**
 * @brief Deserializes plant data from JSON format
 * 
 * @param data JSON string containing serialized plant state
 * 
 * Restores all plant properties from JSON including:
 * - All primitive fields (age, health, waterLevel, etc.)
 * - Enum types (waterRequirement, preferredSeasons)
 * - Polymorphic state object reconstruction
 * 
 * Uses nlohmann::json for parsing and DeserializationUtils for enum conversions
 * and state reconstruction. Preserves the original ID from the save file to
 * maintain object identity across save/load cycles.
 * 
 * @note For Withering state, also reconstructs the previousState reference
 */
void Plant::deserialize(const std::string& data) {
    // Parse JSON using nlohmann/json
    auto json = nlohmann::json::parse(data);

    // Override ID to preserve original from save file
    setId(json["id"].get<uint64_t>());

    // Restore primitive fields
    name = json["name"].get<std::string>();
    price = json["price"].get<double>();
    age = json["age"].get<int>();
    health = json["health"].get<int>();
    waterLevel = json["waterLevel"].get<int>();
    waterConsumption = json["waterConsumption"].get<int>();
    seedlingDuration = json["seedlingDuration"].get<int>();
    growingDuration = json["growingDuration"].get<int>();

    // Parse WaterRequirement enum from string
    waterRequirement =
        DeserializationUtils::parseWaterRequirement(json["waterRequirement"].get<std::string>());

    // Parse preferredSeasons array
    preferredSeasons.clear();
    for (const auto& seasonStr : json["preferredSeasons"]) {
        preferredSeasons.push_back(DeserializationUtils::parseSeason(seasonStr.get<std::string>()));
    }

    // Reconstruct polymorphic state
    if (!json["state"].is_null()) {
        std::string stateType = json["state"]["type"].get<std::string>();

        // Check if Withering state (has previousStateType)
        std::string prevStateType = "";
        if (json["state"].contains("previousStateType")) {
            prevStateType = json["state"]["previousStateType"].get<std::string>();
        }

        currentState = DeserializationUtils::createState(stateType, prevStateType);
    } else {
        currentState = nullptr;
    }
}

/**
 * @brief Returns the type name of this component
 * 
 * @return std::string The string "Plant"
 * 
 * @note Concrete plant subclasses override this to return their specific type
 */
std::string Plant::typeName() const { return "Plant"; }

/**
 * @brief Sets the current growth state of the plant
 * 
 * @param state Unique pointer to the new state (ownership transferred to plant)
 * 
 * Implements the State design pattern by allowing dynamic state changes.
 * The plant takes ownership of the state object and will manage its lifetime.
 */
void Plant::setState(std::unique_ptr<PlantState> state) { currentState = std::move(state); }

/**
 * @brief Executes the plant's daily activities based on current state
 * 
 * Delegates to the current state's performDailyActivity method, which implements
 * state-specific behavior:
 * - Seedling: Age increment, state transition checks
 * - Growing: Age increment, water consumption, health degradation, state transitions
 * - Mature: Water consumption, health degradation, withering checks
 * - Withering: Accelerated health degradation, death checks
 * - Withered: No activity (dead plant)
 * 
 * This method should be called once per simulated day to advance plant lifecycle.
 */
void Plant::performDailyActivity() {
    if (currentState) currentState->performDailyActivity(this);
}

/**
 * @brief Attaches an observer to receive notifications about plant state changes
 * 
 * @param observer Shared pointer to the observer to attach
 * 
 * Implements the Observer design pattern. The observer will be notified via its
 * update() method whenever notify() is called on this plant. Stores observers
 * as weak_ptr to avoid circular references.
 */
void Plant::attach(const std::shared_ptr<Observer>& observer) { observers.push_back(observer); }

/**
 * @brief Detaches an observer from this plant
 * 
 * @param observer Shared pointer to the observer to remove
 * 
 * Removes the specified observer from the notification list. The observer
 * will no longer receive updates about this plant's state changes.
 */
void Plant::detach(const std::shared_ptr<Observer>& observer) {
    observers.erase(
        std::remove_if(observers.begin(), observers.end(),
                       [&observer](const std::weak_ptr<Observer>& weak) {
                           auto shared = weak.lock();  // convert weak pointer to shared pointer
                           return shared && shared == observer;  // remove if observer is matched
                       }),
        observers.end());
}

/**
 * @brief Notifies all attached observers of a state change
 * 
 * Calls update() on each valid observer, passing a shared_ptr to this plant.
 * Automatically removes expired weak_ptr references during iteration, cleaning
 * up observers that have been destroyed.
 * 
 * This method is typically called by state objects when significant changes occur
 * (state transitions, health changes, etc.).
 */
void Plant::notify() {
    // create a shared_potr from 'this' to pass to observers
    auto self = shared_from_this();

    observers.erase(std::remove_if(observers.begin(), observers.end(),
                                   [&self](std::weak_ptr<Observer>& weak) {
                                       auto observer = weak.lock();
                                       if (observer) {
                                           observer->update(self);  // pass the subject(plant)
                                           return false;            // keep it
                                       }
                                       return true;  // remove expired weak_ptr
                                   }),
                    observers.end());
}

/**
 * @brief Removes all observers from this plant
 * 
 * Clears the entire observer list. Useful for cleanup or when transitioning
 * the plant to a state where notifications are no longer needed.
 */
void Plant::detachAllObservers() { observers.clear(); }

/**
 * @brief Applies fertilizer to revive a withering plant
 * 
 * Fertilization can only be applied to plants in the Withering state.
 * It restores the plant's health to 20% and triggers a state change check,
 * potentially returning the plant to its previous healthy state (Growing or Mature).
 * 
 * @note Has no effect if the plant is not in Withering state
 * @note Does not work on Withered (dead) plants
 */
void Plant::fertilize() {
    // Only allow fertilization on Withering plants
    auto witheringState = dynamic_cast<Withering*>(currentState.get());
    if (!witheringState) {
        // Not in Withering state, fertilization has no effect
        return;
    }
    
    setHealth(20);
    currentState->handleStateChange(this);
}

/**
 * @brief Checks if this plant is suitable for growing in the specified season
 * 
 * @param season The season to check suitability for
 * @return bool true if the plant can grow in this season, false otherwise
 * 
 * Plants marked as YEAR_ROUND are suitable for all seasons.
 * Other plants are suitable only during their preferred growing seasons.
 */
bool Plant::isSuitableForSeason(Season season) const {
    // Year-round plants are always suitable
    if (std::find(preferredSeasons.begin(), preferredSeasons.end(), Season::YEAR_ROUND) !=
        preferredSeasons.end()) {
        return true;
    }
    // Check if the requested season is in the plant's preferred seasons
    return std::find(preferredSeasons.begin(), preferredSeasons.end(), season) !=
           preferredSeasons.end();
}

/**
 * @brief Calculates the seasonal price adjustment for this plant
 * 
 * @param currentSeason The current season in the game
 * @return double The adjusted price based on seasonal demand
 * 
 * Pricing logic:
 * - YEAR_ROUND plants: No adjustment (100% of base price)
 * - In-season plants: Bonus of +30% divided by number of preferred seasons
 *   - 1 season: +30%
 *   - 2 seasons: +15% each season
 *   - 3 seasons: +10% each season
 * - Off-season plants: -5% penalty (gentle discount)
 * 
 * This balances multi-season plants so they don't dominate the economy while
 * still providing seasonal variety and trading opportunities.
 */
double Plant::getSeasonalPrice(Season currentSeason) const {
    // Year-round plants have no seasonal adjustment
    if (std::find(preferredSeasons.begin(), preferredSeasons.end(), Season::YEAR_ROUND) !=
        preferredSeasons.end()) {
        return price;
    }

    // Check if current season is one of the plant's preferred seasons
    bool isInSeason = isSuitableForSeason(currentSeason);

    if (isInSeason) {
        // In-season bonus: +30% divided by number of preferred seasons
        // This balances multi-season plants:
        // - 1 season: +30%
        // - 2 seasons: +15%
        // - 3 seasons: +10%
        int numSeasons = static_cast<int>(preferredSeasons.size());
        double bonus = 0.3 / numSeasons;
        return price * (1.0 + bonus);
    } else {
        // Off-season penalty: -5% (gentle, since no winter plants exist)
        return price * 0.95;
    }
}
