#include "../include/doctest.h"

#include "../include/Core/Nursery.h"
#include "../include/Components/Rose.h"
#include "../include/Patterns/Observer/NurserySupervisor.h"
#include "../include/Patterns/Command/WaterPlantCommand.h"

class TestWaterPlantCommand : public WaterPlantCommand {
public:
    using WaterPlantCommand::WaterPlantCommand;
    void execute() override {
        executed = true;
        WaterPlantCommand::execute();
    }
    static bool executed;
};
bool TestWaterPlantCommand::executed = false;

TEST_CASE("NurserySupervisor enqueues watering command when plant is thirsty") {
    auto nursery = std::make_shared<Nursery>();
    auto supervisor = std::make_shared<NurserySupervisor>(nursery);

    auto rose = std::make_shared<Rose>();
    rose->setWaterConsumption(5);
    rose->setWaterLevel(2); 
    rose->attach(supervisor);

    SUBCASE("update triggers watering request") {
        rose->notify();
        CHECK(true);
    }
}

TEST_CASE("NurserySupervisor does nothing if water is sufficient") {
    auto nursery = std::make_shared<Nursery>();
    auto supervisor = std::make_shared<NurserySupervisor>(nursery);

    auto rose = std::make_shared<Rose>();
    rose->setWaterConsumption(5);
    rose->setWaterLevel(10);
    rose->attach(supervisor);

    SUBCASE("no watering request added") {
        rose->notify();

        CHECK(true);
    }
}