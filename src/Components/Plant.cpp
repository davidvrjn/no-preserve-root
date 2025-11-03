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
 * @param name
 * @param price
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

Plant::~Plant() = default;

std::string Plant::getName() const { return name; }

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

std::string Plant::typeName() const { return "Plant"; }

void Plant::setState(std::unique_ptr<PlantState> state) { currentState = std::move(state); }

void Plant::performDailyActivity() {
    if (currentState) currentState->performDailyActivity(this);
}

void Plant::attach(const std::shared_ptr<Observer>& observer) { observers.push_back(observer); }

void Plant::detach(const std::shared_ptr<Observer>& observer) {
    observers.erase(
        std::remove_if(observers.begin(), observers.end(),
                       [&observer](const std::weak_ptr<Observer>& weak) {
                           auto shared = weak.lock();  // convert weak pointer to shared pointer
                           return shared && shared == observer;  // remove if observer is matched
                       }),
        observers.end());
}

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

void Plant::detachAllObservers() { observers.clear(); }

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
