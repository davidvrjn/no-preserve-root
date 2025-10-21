#pragma once
#include "PlantState.h"

class Mature : public PlantState {
public:
    Mature();
    ~Mature() override = default;
    void handleStateChange(Plant* plant) override;
    void performDailyActivity(Plant* plant) override;
    std::unique_ptr<PlantState> clone() const override;
};
