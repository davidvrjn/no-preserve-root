#pragma once
#include "Plant.h"

class Cactus : public Plant {
public:
    Cactus(const std::string& name, double price);
    ~Cactus();

    void water() override;
    InventoryComponent* clone() const override;
};
