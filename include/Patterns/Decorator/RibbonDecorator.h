#pragma once
#include "PlantDecorator.h"

class RibbonDecorator : public PlantDecorator {
public:
    RibbonDecorator(InventoryComponent* component);
    ~RibbonDecorator();

    // Overrides to add the ribbon's price and modify the name.
    std::string getName() const override;
    double getPrice() const override;
};
