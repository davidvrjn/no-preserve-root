
#pragma once
#include "../../Components/InventoryComponent.h"
#include <memory>

/**
 * @class PlantDecorator
 * @brief The abstract base class for all decorators in the Decorator pattern.
 * 
 * It inherits from InventoryComponent, which is crucial. This ensures that a
 * decorated plant is still treated as a valid InventoryComponent by the rest of
 * the system. It also holds a shared_ptr to the InventoryComponent it wraps,
 * allowing decorators to be chained.
 */
class PlantDecorator : public InventoryComponent {
protected:
    std::shared_ptr<InventoryComponent> wrappedComponent; // The component being decorated.

public:
    PlantDecorator(const std::shared_ptr<InventoryComponent>& component);
    ~PlantDecorator() override = default;

    // The decorator delegates the core functionality calls to the wrapped component.
    // Concrete decorators will override these methods to add their own behavior.
    std::string getName() const override;
    double getPrice() const override;
    std::unique_ptr<Iterator> createIterator() override;
    std::shared_ptr<InventoryComponent> clone() const override;
    std::shared_ptr<InventoryComponent> blueprintClone() const override;
    std::string serialize() const override;
    void deserialize(const std::string& data) override;
    std::string typeName() const override;
};
