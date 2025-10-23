#include "../../../include/Patterns/Decorator/GiftWrapDecorator.h"

GiftWrapDecorator::GiftWrapDecorator(const std::shared_ptr<InventoryComponent>& component)
    : PlantDecorator(component) {}

std::string GiftWrapDecorator::getName() const {
    return wrappedComponent ? wrappedComponent->getName() + " + Gift Wrap" : std::string("Gift Wrap");
}
double GiftWrapDecorator::getPrice() const {
    return wrappedComponent ? wrappedComponent->getPrice() + 15.0 : 15.0;
}
std::shared_ptr<InventoryComponent> GiftWrapDecorator::blueprintClone() const { 
    return std::make_shared<GiftWrapDecorator>(wrappedComponent ? wrappedComponent->blueprintClone() : nullptr);
}
std::string GiftWrapDecorator::serialize() const { 
    return "GiftWrap|" + (wrappedComponent ? wrappedComponent->serialize() : "");
}
void GiftWrapDecorator::deserialize(const std::string& data) { 
    auto pos = data.find('|');
    if (pos != std::string::npos && wrappedComponent) {
        wrappedComponent->deserialize(data.substr(pos + 1));
    }
}
std::string GiftWrapDecorator::typeName() const { return "GiftWrapDecorator"; }
