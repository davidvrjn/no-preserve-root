#include "../../include/Core/PlantRegistry.h"

#include <stdexcept>

#include "../../include/Components/Plant.h"

// Static registry stored in function to ensure initialization
std::unordered_map<std::string, std::function<std::shared_ptr<Plant>()>>&
PlantRegistry::getRegistry() {
    static std::unordered_map<std::string, std::function<std::shared_ptr<Plant>()>> registry;
    return registry;
}

void PlantRegistry::registerType(const std::string& type,
                                 std::function<std::shared_ptr<Plant>()> factory) {
    getRegistry()[type] = factory;
}

std::shared_ptr<Plant> PlantRegistry::create(const std::string& type) {
    auto& registry = getRegistry();
    auto it = registry.find(type);
    if (it == registry.end()) {
        throw std::out_of_range("Unknown plant type: " + type);
    }
    return it->second();
}

bool PlantRegistry::isRegistered(const std::string& type) {
    return getRegistry().find(type) != getRegistry().end();
}
