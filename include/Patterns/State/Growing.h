#pragma once
#include "PlantState.h"

class Growing : public PlantState {
public:
    Growing();
    ~Growing() override = default;
    void handleStateChange(Plant* plant) override;
    void performDailyActivity(Plant* plant) override;
    std::unique_ptr<PlantState> clone() const override;
};
