#include "../../../include/Patterns/Decorator/GiftWrapDecorator.h"

GiftWrapDecorator::GiftWrapDecorator(const std::shared_ptr<InventoryComponent>& component)
    : PlantDecorator(component) {}

std::string GiftWrapDecorator::getName() const {
    return wrappedComponent ? wrappedComponent->getName() : std::string();
}
double GiftWrapDecorator::getPrice() const {
    return wrappedComponent ? wrappedComponent->getPrice() : 0.0;
}
std::shared_ptr<InventoryComponent> GiftWrapDecorator::blueprintClone() const { return nullptr; }
std::string GiftWrapDecorator::serialize() const { return std::string(); }
void GiftWrapDecorator::deserialize(const std::string& data) { (void)data; }
std::string GiftWrapDecorator::typeName() const { return "GiftWrapDecorator"; }
