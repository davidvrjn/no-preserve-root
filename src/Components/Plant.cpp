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

std::string Plant::getName() const { return name; }

double Plant::getPrice() const { return price; }

std::unique_ptr<Iterator> Plant::createIterator() { return nullptr; }

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
