#pragma once
#include "PlantState.h"

class Withered : public PlantState {
public:
    Withered();
    ~Withered();
    void handleStateChange(Plant* plant) override;
    void performDailyActivity(Plant* plant) override;
    PlantState* clone() const override;
};
