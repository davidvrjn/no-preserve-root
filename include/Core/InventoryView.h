#pragma once

#include <memory>
#include <string>
#include <vector>

#include "../Components/InventoryComponent.h"

class Inventory;
class Plant;
class Group;

namespace UI {

struct PlantView {
    uint64_t id{0};
    std::string name;
    std::string type;
    int age{0};
    std::string state;
};

/**
 * @class InventoryView
 * @brief A thin read-only adapter that exposes inventory data for UI presentation.
 *
 * Responsibilities:
 *  - List available top-level groups (storage and plots)
 *  - Return a list of plants for a given group name
 *  - Provide lightweight summaries useful for UIs (counts, names, types)
 *
 * This class intentionally doesn't modify the game state. It is read-only and
 * performs safe casts to extract plant-specific information.
 */
class InventoryView {
   public:
    explicit InventoryView(const std::shared_ptr<Inventory>& inventory);

    // Return names of top-level groups (Storage, Plot A.. etc.)
    std::vector<std::string> listGroupNames() const;

    // Return plants in the given group (empty vector if not found)
    std::vector<PlantView> listPlantsInGroup(const std::string& groupName) const;

    // Convenience accessor for storage group
    std::vector<PlantView> listStoragePlants() const;

   private:
    std::shared_ptr<Inventory> inventory_;

    // Helper to format a plant into PlantView
    static PlantView makePlantView(const std::shared_ptr<Plant>& plant);
};

}  // namespace UI
