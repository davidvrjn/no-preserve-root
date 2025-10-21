#pragma once
#include "PlantState.h"

class Seedling : public PlantState {
public:
    Seedling();
    ~Seedling();
    void handleStateChange(Plant* plant) override;
    void performDailyActivity(Plant* plant) override;
    PlantState* clone() const override;
};
