#include "../../../include/Patterns/Decorator/RibbonDecorator.h"

/**
 * @brief Construct a new Ribbon Decorator:: Ribbon Decorator object
 * 
 * @param component Shared pointer to the inventory component to be decorated with a pot.
 * 
 * This decorator places a plant or other inventory item in a decorative pot,
 * increasing its price and enhancing its name display.
 */
RibbonDecorator::RibbonDecorator(const std::shared_ptr<InventoryComponent>& component)
    : PlantDecorator(component) {}

/**
 * @brief Gets the decorated name of the component with pot indication.
 * 
 * @return std::string The component's name with " in Pot" appended,
 *         or "Pot" if no component is wrapped.
 */
std::string RibbonDecorator::getName() const {
    return wrappedComponent ? wrappedComponent->getName() + " with Ribbon" : std::string("Ribbon");
}

/**
 * @brief Gets the price of the component including the pot cost.
 * 
 * @return double The base component price plus 30.0 for the decorative pot,
 *         or 30.0 if no component is wrapped.
 */
double RibbonDecorator::getPrice() const {
    return wrappedComponent ? wrappedComponent->getPrice() + 10.0 : 10.0;
}

/**
 * @brief Creates a deep copy of this decorator and its wrapped component.
 * 
 * @return std::shared_ptr<InventoryComponent> A new PotDecorator wrapping
 *         a cloned copy of the original component, or nullptr if no component exists.
 */
std::shared_ptr<InventoryComponent> RibbonDecorator::clone() const {
    if (!wrappedComponent) return nullptr;
    return std::make_shared<RibbonDecorator>(wrappedComponent->clone());
}

/**
 * @brief Creates a blueprint copy of this decorator and its wrapped component.
 * 
 * Blueprint clones are typically used for creating template instances without
 * full state information.
 * 
 * @return std::shared_ptr<InventoryComponent> A new PotDecorator wrapping
 *         a blueprint clone of the original component, or nullptr if no component exists.
 */
std::shared_ptr<InventoryComponent> RibbonDecorator::blueprintClone() const {
    if (!wrappedComponent) return nullptr;
    return std::make_shared<RibbonDecorator>(wrappedComponent->blueprintClone());
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
std::string RibbonDecorator::serialize() const {
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
void RibbonDecorator::deserialize(const std::string& data) { (void)data; }

/**
 * @brief Gets the type name of this decorator.
 * 
 * @return std::string The string "PotDecorator" identifying this decorator type.
 */
std::string RibbonDecorator::typeName() const { return "RibbonDecorator"; }
