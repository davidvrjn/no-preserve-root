#include "../../../include/Patterns/Factory/PetuniaFactory.h"

#include "../../../include/Components/Petunia.h"

PetuniaFactory::PetuniaFactory() = default;

std::shared_ptr<Plant> PetuniaFactory::createPlant() { return std::make_shared<Petunia>(); }
