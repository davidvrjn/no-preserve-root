#include "../../../include/Patterns/Decorator/RibbonDecorator.h"

RibbonDecorator::RibbonDecorator(const std::shared_ptr<InventoryComponent>& component)
    : PlantDecorator(component) {}

std::string RibbonDecorator::getName() const {
    return wrappedComponent ? wrappedComponent->getName() : std::string();
}
double RibbonDecorator::getPrice() const {
    return wrappedComponent ? wrappedComponent->getPrice() : 0.0;
}
std::shared_ptr<InventoryComponent> RibbonDecorator::blueprintClone() const { return nullptr; }
std::string RibbonDecorator::serialize() const { return std::string(); }
void RibbonDecorator::deserialize(const std::string& data) { (void)data; }
std::string RibbonDecorator::typeName() const { return "RibbonDecorator"; }
