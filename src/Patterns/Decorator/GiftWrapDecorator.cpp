#include "../../../include/Patterns/Decorator/GiftWrapDecorator.h"

/**
 * @brief Construct a new Gift Wrap Decorator:: Gift Wrap Decorator object
 * 
 * @param component Shared pointer to the inventory component to be decorated with gift wrapping.
 * 
 * This decorator adds gift wrap presentation to a plant or other inventory item,
 * increasing its price and enhancing its name display
 * 
 */
GiftWrapDecorator::GiftWrapDecorator(const std::shared_ptr<InventoryComponent>& component)
    : PlantDecorator(component) {}

 /**
  * @brief Gets the decorated name of the component with gift wrap indication.
  * 
  * @return std::string The component's name with " + Gift Wrap" appended,
  *         or "Gift Wrap" if no component is wrapped.
  */
std::string GiftWrapDecorator::getName() const {
    return wrappedComponent ? wrappedComponent->getName() + " + Gift Wrap"
                            : std::string("Gift Wrap");
}

/**
 * @brief Gets the price of the component including the gift wrap cost.
 * 
 * @return double The base component price plus 15.0 for gift wrapping,
 *         or 15.0 if no component is wrapped.
 */
double GiftWrapDecorator::getPrice() const {
    return wrappedComponent ? wrappedComponent->getPrice() + 15.0 : 15.0;
}

/**
 * @brief Creates a deep copy of this decorator and its wrapped component.
 * 
 * @return std::shared_ptr<InventoryComponent> A new GiftWrapDecorator wrapping
 *         a cloned copy of the original component, or nullptr if no component exists.
 */
std::shared_ptr<InventoryComponent> GiftWrapDecorator::clone() const {
    if (!wrappedComponent) return nullptr;
    return std::make_shared<GiftWrapDecorator>(wrappedComponent->clone());
}

/**
 * @brief Creates a blueprint copy of this decorator and its wrapped component.
 * 
 * Blueprint clones are typically used for creating template instances without
 * full state information.
 * 
 * @return std::shared_ptr<InventoryComponent> A new GiftWrapDecorator wrapping
 *         a blueprint clone of the original component, or nullptr if no component exists.
 */
std::shared_ptr<InventoryComponent> GiftWrapDecorator::blueprintClone() const {
    if (!wrappedComponent) return nullptr;
    return std::make_shared<GiftWrapDecorator>(wrappedComponent->blueprintClone());
}

/**
 * @brief Serializes the decorator to a string representation.
 * 
 * @return std::string An empty string, as decorators are transient and only
 *         exist during customer transactions. They are not persisted.
 * 
 * @note Decorators are applied temporarily during purchase fulfillment and
 *       are not saved to storage or included in inventory serialization.
 */
std::string GiftWrapDecorator::serialize() const {
    // Decorators are transient (only exist during customer transactions)
    return std::string();
}

/**
 * @brief Deserializes data into this decorator.
 * 
 * @param data The serialization data (unused for decorators).
 * 
 * @note This method is a no-op because decorators are transient and not persisted.
 */
void GiftWrapDecorator::deserialize(const std::string& data) { (void)data; }

/**
 * @brief Gets the type name of this decorator.
 * 
 * @return std::string The string "GiftWrapDecorator" identifying this decorator type.
 */
std::string GiftWrapDecorator::typeName() const { return "GiftWrapDecorator"; }
