#include "../../../include/Patterns/Factory/LavenderFactory.h"

#include "../../../include/Components/Lavender.h"

LavenderFactory::LavenderFactory() = default;

std::shared_ptr<Plant> LavenderFactory::createPlant() { return std::make_shared<Lavender>(); }
