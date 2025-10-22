#include "../../../include/Patterns/Decorator/PlantDecorator.h"

#include "../../../include/Patterns/Iterator/Iterator.h"

PlantDecorator::PlantDecorator(const std::shared_ptr<InventoryComponent>& component)
    : wrappedComponent(component) {}

std::string PlantDecorator::getName() const {
    return wrappedComponent ? wrappedComponent->getName() : std::string();
}

double PlantDecorator::getPrice() const {
    return wrappedComponent ? wrappedComponent->getPrice() : 0.0;
}

std::unique_ptr<Iterator> PlantDecorator::createIterator() {
    return wrappedComponent ? wrappedComponent->createIterator() : nullptr;
}

std::shared_ptr<InventoryComponent> PlantDecorator::clone() const {
    return wrappedComponent ? wrappedComponent->clone() : nullptr;
}

std::shared_ptr<InventoryComponent> PlantDecorator::blueprintClone() const {
    return wrappedComponent ? wrappedComponent->blueprintClone() : nullptr;
}

std::string PlantDecorator::serialize() const {
    return wrappedComponent ? wrappedComponent->serialize() : std::string();
}

void PlantDecorator::deserialize(const std::string& data) {
    if (wrappedComponent) wrappedComponent->deserialize(data);
}

std::string PlantDecorator::typeName() const { return "PlantDecorator"; }
