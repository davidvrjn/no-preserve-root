#pragma once
#include "PlantDecorator.h"

class PotDecorator : public PlantDecorator {
public:
    PotDecorator(InventoryComponent* component);
    ~PotDecorator();

    // Overrides to add the pot's price and modify the name.
    std::string getName() const override;
    double getPrice() const override;
};
