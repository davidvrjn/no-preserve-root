#include "../../../include/Patterns/Decorator/PotDecorator.h"

PotDecorator::PotDecorator(const std::shared_ptr<InventoryComponent>& component)
    : PlantDecorator(component) {}

std::string PotDecorator::getName() const {
    return wrappedComponent ? wrappedComponent->getName() + " in Pot" : std::string("Pot");
}
double PotDecorator::getPrice() const {
    return wrappedComponent ? wrappedComponent->getPrice() + 30.0 : 30.0;
}

std::shared_ptr<InventoryComponent> PotDecorator::clone() const {
    if (!wrappedComponent) return nullptr;
    return std::make_shared<PotDecorator>(wrappedComponent->clone());
}
std::shared_ptr<InventoryComponent> PotDecorator::blueprintClone() const { 
    if (!wrappedComponent) return nullptr;
    return std::make_shared<PotDecorator>(wrappedComponent->blueprintClone());
 }
std::string PotDecorator::serialize() const { return std::string();}
void PotDecorator::deserialize(const std::string& data) { (void)data; }
std::string PotDecorator::typeName() const { return "PotDecorator"; }
