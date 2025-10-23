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
        explicit LeafIterator(std::shared_ptr<InventoryComponent> comp) : component(std::move(comp)) {}
        
        std::shared_ptr<InventoryComponent> next() override {
            if (!consumed && component) {
                consumed = true;
                return component;
            }
            return nullptr;
        }
        
        bool hasNext() const override {
            return !consumed && component != nullptr;
        }
    };
    
    // Get shared_ptr to this plant through Subject base, then cast to InventoryComponent
    auto subjectPtr = Subject::shared_from_this();
    auto inventoryPtr = std::dynamic_pointer_cast<InventoryComponent>(subjectPtr);
    return std::make_unique<LeafIterator>(inventoryPtr);
}

std::shared_ptr<InventoryComponent> Plant::clone() const { return nullptr; }

std::shared_ptr<InventoryComponent> Plant::blueprintClone() const { return nullptr; }

std::string Plant::serialize() const { return std::string(); }

void Plant::deserialize(const std::string& data) { (void)data; }

std::string Plant::typeName() const { return "Plant"; }

void Plant::setState(std::unique_ptr<PlantState> state) { currentState = std::move(state); }

void Plant::performDailyActivity() {
    if (currentState) currentState->performDailyActivity(this);
}

void Plant::attach(const std::shared_ptr<Observer>& observer) { (void)observer; }

void Plant::detach(const std::shared_ptr<Observer>& observer) { (void)observer; }

void Plant::notify() { /* stub */
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
