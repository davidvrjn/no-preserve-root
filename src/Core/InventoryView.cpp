#include "../../include/Core/InventoryView.h"

#include <algorithm>

#include "../../include/Components/Group.h"
#include "../../include/Components/Plant.h"
#include "../../include/Core/Inventory.h"
#include "../../include/Patterns/State/Growing.h"
#include "../../include/Patterns/State/Mature.h"
#include "../../include/Patterns/State/Seedling.h"
#include "../../include/Patterns/State/Withered.h"
#include "../../include/Patterns/State/Withering.h"

using namespace UI;

InventoryView::InventoryView(const std::shared_ptr<Inventory>& inventory) : inventory_(inventory) {}

std::vector<std::string> InventoryView::listGroupNames() const {
    std::vector<std::string> names;
    if (!inventory_) return names;

    auto groups = inventory_->getAllGroups();
    bool hasStorage = false;
    std::vector<std::string> others;
    for (const auto& g : groups) {
        if (!g) continue;
        // Filter out the temporary traversal root used by Inventory (InventoryRoot)
        auto nm = g->getName();
        if (nm == "InventoryRoot") continue;
        if (nm == "Storage") {
            hasStorage = true;
        } else {
            others.push_back(nm);
        }
    }

    // Sort the remaining groups alphabetically
    std::sort(others.begin(), others.end());

    // Put Storage first if present, then the sorted others
    if (hasStorage) names.push_back("Storage");
    names.insert(names.end(), others.begin(), others.end());
    return names;
}

std::vector<PlantView> InventoryView::listPlantsInGroup(const std::string& groupName) const {
    std::vector<PlantView> result;
    if (!inventory_) return result;

    auto group = inventory_->findGroupByName(groupName);
    if (!group) return result;

    auto members = group->members();
    for (const auto& comp : members) {
        if (!comp) continue;
        auto plant = std::dynamic_pointer_cast<Plant>(comp);
        if (!plant) continue;
        result.push_back(makePlantView(plant));
    }
    return result;
}

std::vector<PlantView> InventoryView::listStoragePlants() const {
    return listPlantsInGroup("Storage");
}

PlantView InventoryView::makePlantView(const std::shared_ptr<Plant>& plant) {
    PlantView v;
    if (!plant) return v;
    v.id = plant->getId();
    v.name = plant->getName();
    v.type = plant->typeName();
    v.age = plant->getAge();

    // Determine state by RTTI on PlantState
    auto state = plant->getState();
    if (!state) {
        v.state = "(none)";
    } else if (dynamic_cast<const Seedling*>(state)) {
        v.state = "Seedling";
    } else if (dynamic_cast<const Growing*>(state)) {
        v.state = "Growing";
    } else if (dynamic_cast<const Mature*>(state)) {
        v.state = "Mature";
    } else if (dynamic_cast<const Withering*>(state)) {
        v.state = "Withering";
    } else if (dynamic_cast<const Withered*>(state)) {
        v.state = "Withered";
    } else {
        v.state = "Unknown";
    }

    return v;
}
