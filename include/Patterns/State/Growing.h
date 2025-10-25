#pragma once
#include "PlantState.h"

//Transitions
#include "Mature.h"
#include "Withering.h"

class Growing : public PlantState {
   public:
    Growing();
    ~Growing() override = default;
    void handleStateChange(Plant* plant) override;
    void performDailyActivity(Plant* plant) override;
    std::unique_ptr<PlantState> clone() const override;
};
