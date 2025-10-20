#pragma once
#include "PlantDecorator.h"

class GiftWrapDecorator : public PlantDecorator {
public:
    GiftWrapDecorator(InventoryComponent* component);
    ~GiftWrapDecorator();

    // Overrides to add the gift wrap's price and modify the name.
    std::string getName() const override;
    double getPrice() const override;
};
