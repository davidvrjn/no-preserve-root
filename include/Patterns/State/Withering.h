#pragma once
#include "PlantState.h"

class Withering : public PlantState {
   public:
    Withering(PlantState* prev){previousState = prev;};
    ~Withering() override = default;
    void handleStateChange(Plant* plant) override;
    void performDailyActivity(Plant* plant) override;
    std::unique_ptr<PlantState> clone() const override;
    private:
    PlantState* previousState;
};
