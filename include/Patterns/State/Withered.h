#pragma once
#include "PlantState.h"

class Withered : public PlantState {
public:
    Withered();
    ~Withered() override = default;
    void handleStateChange(Plant* plant) override;
    void performDailyActivity(Plant* plant) override;
    std::unique_ptr<PlantState> clone() const override;
};
