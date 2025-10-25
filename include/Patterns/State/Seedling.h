#pragma once
#include "PlantState.h"

//Include states that can be transitioned to
#include "Growing.h"
#include "Withering.h"

class Seedling : public PlantState {
   public:
    Seedling();
    ~Seedling() override = default;
    void handleStateChange(Plant* plant) override;
    void performDailyActivity(Plant* plant) override;
    std::unique_ptr<PlantState> clone() const override;
};
