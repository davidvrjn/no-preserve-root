#pragma once
#include "PlantState.h"

class Growing : public PlantState {
public:
    Growing();
    ~Growing();
    void handleStateChange(Plant* plant) override;
    void performDailyActivity(Plant* plant) override;
    PlantState* clone() const override;
};
