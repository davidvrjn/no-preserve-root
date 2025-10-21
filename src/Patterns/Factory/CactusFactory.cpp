#include "../../../include/Patterns/Factory/CactusFactory.h"
#include "../../../include/Components/Cactus.h"

CactusFactory::CactusFactory() = default;

std::shared_ptr<Plant> CactusFactory::createPlant() {
	return nullptr;
}

