#include "../../../include/Patterns/Factory/RoseFactory.h"

#include "../../../include/Components/Rose.h"
#include "../../../include/Patterns/State/Seedling.h"

RoseFactory::RoseFactory() = default;

std::shared_ptr<Plant> RoseFactory::createPlant() {
    auto rose = std::make_shared<Rose>();
    // All newly planted plants start in Seedling state
    rose->setState(std::make_unique<Seedling>());
    return rose;
}
