#pragma once
#include "PlantState.h"
#include "Withered.h"

class Withering : public PlantState {
   public:
    Withering(std::unique_ptr<PlantState> prev);
    ~Withering() override = default;
    void handleStateChange(Plant* plant) override;
    void performDailyActivity(Plant* plant) override;
    std::unique_ptr<PlantState> clone() const override;

    // Helper for serialization - returns the previous state pointer
    const PlantState* getPreviousState() const { return previousState.get(); }

   private:
    std::unique_ptr<PlantState> previousState;
};
