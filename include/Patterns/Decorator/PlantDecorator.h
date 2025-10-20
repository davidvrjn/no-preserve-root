#pragma once
#include "../../Components/InventoryComponent.h"

/**
 * @class PlantDecorator
 * @brief The abstract base class for all decorators in the Decorator pattern.
 * 
 * It inherits from InventoryComponent, which is crucial. This ensures that a
 * decorated plant is still treated as a valid InventoryComponent by the rest of
 * the system. It also holds a pointer to the InventoryComponent it wraps,
 * allowing decorators to be chained.
 */
class PlantDecorator : public InventoryComponent {
protected:
    InventoryComponent* wrappedComponent; // The component being decorated.

public:
    PlantDecorator(InventoryComponent* component);
    virtual ~PlantDecorator();

    // The decorator delegates the core functionality calls to the wrapped component.
    // Concrete decorators will override these methods to add their own behavior.
    std::string getName() const override;
    double getPrice() const override;
    Iterator* createIterator() override;
    InventoryComponent* clone() const override;
};
