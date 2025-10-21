#pragma once
#include "PlantState.h"

class Mature : public PlantState {
public:
    Mature();
    ~Mature();
    void handleStateChange(Plant* plant) override;
    void performDailyActivity(Plant* plant) override;
    PlantState* clone() const override;
};
