#include "../../../include/Patterns/Decorator/PotDecorator.h"

PotDecorator::PotDecorator(const std::shared_ptr<InventoryComponent>& component)
	: PlantDecorator(component) {}

std::string PotDecorator::getName() const { return wrappedComponent ? wrappedComponent->getName() : std::string(); }
double PotDecorator::getPrice() const { return wrappedComponent ? wrappedComponent->getPrice() : 0.0; }
std::shared_ptr<InventoryComponent> PotDecorator::blueprintClone() const { return nullptr; }
std::string PotDecorator::serialize() const { return std::string(); }
void PotDecorator::deserialize(const std::string& data) { (void)data; }
std::string PotDecorator::typeName() const { return "PotDecorator"; }

