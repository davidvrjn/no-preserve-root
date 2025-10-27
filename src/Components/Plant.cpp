#include "../../include/Components/Plant.h"

#include <algorithm>

#include "../../include/Patterns/Iterator/Iterator.h"
#include "../../include/Patterns/Observer/Observer.h"
#include "../../include/Patterns/State/PlantState.h"

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

std::string Plant::serialize() const { return std::string(); }

void Plant::deserialize(const std::string& data) { (void)data; }

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
    int current = getHealth();
    setHealth(std::min(100, current + 20));  // Restore 20 health, max 100
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
