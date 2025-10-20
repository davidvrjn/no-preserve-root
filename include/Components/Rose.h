#pragma once
#include "Plant.h"

class Rose : public Plant {
public:
    Rose(const std::string& name, double price);
    ~Rose();

    void water() override;
    InventoryComponent* clone() const override;
};
