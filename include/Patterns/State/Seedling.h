#pragma once
#include "PlantState.h"

class Seedling : public PlantState {
public:
    Seedling();
    ~Seedling() override = default;
    void handleStateChange(Plant* plant) override;
    void performDailyActivity(Plant* plant) override;
    std::unique_ptr<PlantState> clone() const override;
};
