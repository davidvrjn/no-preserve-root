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

std::string PlantDecorator::serialize() const {
    // Decorators are transient (only exist during customer transactions)
    // They are never saved to inventory, so serialization is not needed
    return std::string();
}

void PlantDecorator::deserialize(const std::string& data) {
    // Decorators are transient - no deserialization needed
    (void)data;
}

std::string PlantDecorator::typeName() const { return "PlantDecorator"; }
