#include "../../include/Core/DeserializationUtils.h"

#include "../../include/Patterns/State/Growing.h"
#include "../../include/Patterns/State/Mature.h"
#include "../../include/Patterns/State/PlantState.h"
#include "../../include/Patterns/State/Seedling.h"
#include "../../include/Patterns/State/Withered.h"
#include "../../include/Patterns/State/Withering.h"

namespace DeserializationUtils {

std::unique_ptr<PlantState> createState(const std::string& type,
                                        const std::string& previousStateType) {
    if (type == "Seedling") {
        return std::make_unique<Seedling>();
    }
    if (type == "Growing") {
        return std::make_unique<Growing>();
    }
    if (type == "Mature") {
        return std::make_unique<Mature>();
    }
    if (type == "Withered") {
        return std::make_unique<Withered>();
    }
    if (type == "Withering") {
        // Withering requires previous state
        if (previousStateType.empty()) {
            throw std::invalid_argument("Withering state requires previousStateType");
        }
        auto prevState = createState(previousStateType);
        return std::make_unique<Withering>(std::move(prevState));
    }

    throw std::invalid_argument("Unknown PlantState type: " + type);
}

}  // namespace DeserializationUtils
