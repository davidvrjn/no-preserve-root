#pragma once
#include "PlantState.h"

class Withering : public PlantState {
public:
    Withering();
    ~Withering();
    void handleStateChange(Plant* plant) override;
    void performDailyActivity(Plant* plant) override;
    PlantState* clone() const override;
};
