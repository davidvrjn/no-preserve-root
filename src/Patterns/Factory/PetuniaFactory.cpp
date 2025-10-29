#include "../../../include/Patterns/Factory/PetuniaFactory.h"

#include "../../../include/Components/Petunia.h"
#include "../../../include/Patterns/State/Seedling.h"

PetuniaFactory::PetuniaFactory() = default;

std::shared_ptr<Plant> PetuniaFactory::createPlant() {
    auto plant = std::make_shared<Petunia>();
    plant->setState(std::make_unique<Seedling>());
    return plant;
}
