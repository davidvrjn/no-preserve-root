#include "../../../include/Patterns/Decorator/PotDecorator.h"

/**
 * @brief Constructs a RibbonDecorator wrapping an inventory component.
 * 
 * @param component Shared pointer to the inventory component to be decorated with a ribbon.
 * 
 * This decorator adds a decorative ribbon to a plant or other inventory item,
 * increasing its price and enhancing its name display.
 */
PotDecorator::PotDecorator(const std::shared_ptr<InventoryComponent>& component)
    : PlantDecorator(component) {}

/**
 * @brief Gets the decorated name of the component with ribbon indication.
 * 
 * @return std::string The component's name with " with Ribbon" appended,
 *         or "Ribbon" if no component is wrapped.
 */
std::string PotDecorator::getName() const {
    return wrappedComponent ? wrappedComponent->getName() + " in Pot" : std::string("Pot");
}

/**
 * @brief Gets the price of the component including the ribbon cost.
 * 
 * @return double The base component price plus 10.0 for the ribbon decoration,
 *         or 10.0 if no component is wrapped.
 */
double PotDecorator::getPrice() const {
    return wrappedComponent ? wrappedComponent->getPrice() + 30.0 : 30.0;
}

/**
 * @brief Creates a deep copy of this decorator and its wrapped component.
 * 
 * @return std::shared_ptr<InventoryComponent> A new RibbonDecorator wrapping
 *         a cloned copy of the original component, or nullptr if no component exists.
 */
std::shared_ptr<InventoryComponent> PotDecorator::clone() const {
    if (!wrappedComponent) return nullptr;
    return std::make_shared<PotDecorator>(wrappedComponent->clone());
}

/**
 * @brief Creates a blueprint copy of this decorator and its wrapped component.
 * 
 * Blueprint clones are typically used for creating template instances without
 * full state information.
 * 
 * @return std::shared_ptr<InventoryComponent> A new RibbonDecorator wrapping
 *         a blueprint clone of the original component, or nullptr if no component exists.
 */
std::shared_ptr<InventoryComponent> PotDecorator::blueprintClone() const {
    if (!wrappedComponent) return nullptr;
    return std::make_shared<PotDecorator>(wrappedComponent->blueprintClone());
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
std::string PotDecorator::serialize() const {
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
void PotDecorator::deserialize(const std::string& data) { (void)data; }

/**
 * @brief Gets the type name of this decorator.
 * 
 * @return std::string The string "RibbonDecorator" identifying this decorator type.
 */
std::string PotDecorator::typeName() const { return "PotDecorator"; }
