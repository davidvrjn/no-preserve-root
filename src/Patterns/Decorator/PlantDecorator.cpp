#include "../../../include/Patterns/Decorator/PlantDecorator.h"

#include "../../../include/Patterns/Iterator/Iterator.h"

/**
 * @brief Constructs a PlantDecorator wrapping an inventory component.
 * 
 * @param component Shared pointer to the inventory component to be decorated.
 * 
 * This is the base class for all plant decorators (GiftWrap, Pot, Ribbon).
 * It provides default implementations that delegate to the wrapped component.
 */
PlantDecorator::PlantDecorator(const std::shared_ptr<InventoryComponent>& component)
    : wrappedComponent(component) {}

/**
 * @brief Gets the name of the wrapped component.
 * 
 * @return std::string The name of the wrapped component, or an empty string
 *         if no component is wrapped.
 * 
 * @note Derived decorators typically override this to add decoration-specific text.
 */
std::string PlantDecorator::getName() const {
    return wrappedComponent ? wrappedComponent->getName() : std::string();
}

/**
 * @brief Gets the price of the wrapped component.
 * 
 * @return double The price of the wrapped component, or 0.0 if no component is wrapped.
 * 
 * @note Derived decorators typically override this to add their specific cost.
 */
double PlantDecorator::getPrice() const {
    return wrappedComponent ? wrappedComponent->getPrice() : 0.0;
}

/**
 * @brief Creates an iterator for the wrapped component.
 * 
 * @return std::unique_ptr<Iterator> An iterator for the wrapped component,
 *         or nullptr if no component is wrapped.
 * 
 * @note This allows decorated components to maintain their composite structure
 *       and iteration capabilities.
 */
std::unique_ptr<Iterator> PlantDecorator::createIterator() {
    return wrappedComponent ? wrappedComponent->createIterator() : nullptr;
}

/**
 * @brief Serializes the decorator to a string representation.
 * 
 * @return std::string An empty string, as decorators are transient.
 * 
 * @note Decorators are transient and only exist during customer transactions.
 *       They are never saved to inventory, so serialization is not needed.
 */
std::string PlantDecorator::serialize() const {
    // Decorators are transient (only exist during customer transactions)
    // They are never saved to inventory, so serialization is not needed
    return std::string();
}

/**
 * @brief Deserializes data into this decorator.
 * 
 * @param data The serialization data (unused for decorators).
 * 
 * @note Decorators are transient - no deserialization needed as they are
 *       never persisted to storage.
 */
void PlantDecorator::deserialize(const std::string& data) {
    // Decorators are transient - no deserialization needed
    (void)data;
}

/**
 * @brief Gets the type name of this decorator.
 * 
 * @return std::string The string "PlantDecorator" identifying this base decorator type.
 * 
 * @note Derived classes override this to return their specific type name
 *       (e.g., "GiftWrapDecorator", "PotDecorator").
 */
std::string PlantDecorator::typeName() const { return "PlantDecorator"; }
