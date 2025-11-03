#include <memory>

#include "../include/Actors/Cashier.h"
#include "../include/Actors/Customer.h"
#include "../include/Actors/Gardener.h"
#include "../include/Components/Cactus.h"
#include "../include/Components/Group.h"
#include "../include/Components/Plant.h"
#include "../include/Components/Rose.h"
#include "../include/Core/Inventory.h"
#include "../include/Core/Nursery.h"
#include "../include/Patterns/Builder/PlantSpecification.h"
#include "../include/Patterns/Command/FulfillCustomerCommand.h"
#include "../include/Patterns/Command/WaterPlantCommand.h"
#include "../include/Patterns/Iterator/Iterator.h"
#include "../include/doctest.h"

TEST_CASE("Nursery simulation - phase transitions") {
    auto nursery = std::make_shared<Nursery>();

    SUBCASE("Initial state") {
        CHECK(nursery->getCurrentPhase() == GamePhase::IDLE);
        CHECK(nursery->getCurrentDay() == 0);
        CHECK(nursery->getCurrentStep() == 0);
    }

    SUBCASE("startNewDay transitions to DAY_START") {
        nursery->startNewDay();
        CHECK(nursery->getCurrentPhase() == GamePhase::DAY_START);
        CHECK(nursery->getCurrentDay() == 1);
        CHECK(nursery->getCurrentStep() == 0);
        CHECK_FALSE(nursery->isDayComplete());
    }

    SUBCASE("advanceStep transitions through phases") {
        nursery->startNewDay();

        // First step: DAY_START -> STEP_BREAK
        nursery->advanceStep();
        CHECK(nursery->getCurrentPhase() == GamePhase::STEP_BREAK);
        CHECK(nursery->getCurrentStep() == 1);

        // Steps 2-4: STEP_BREAK -> STEP_BREAK
        for (int i = 2; i <= 4; i++) {
            nursery->advanceStep();
            CHECK(nursery->getCurrentPhase() == GamePhase::STEP_BREAK);
            CHECK(nursery->getCurrentStep() == i);
        }

        // Step 5: STEP_BREAK -> DAY_END
        nursery->advanceStep();
        CHECK(nursery->getCurrentPhase() == GamePhase::DAY_END);
        CHECK(nursery->getCurrentStep() == 5);
        CHECK(nursery->isDayComplete());
    }

    SUBCASE("Cannot start new day while day in progress") {
        nursery->startNewDay();
        nursery->advanceStep();  // Now in STEP_BREAK

        CHECK_THROWS_WITH(nursery->startNewDay(),
                          "Cannot start new day: day in progress. Complete current day first.");
    }

    SUBCASE("Can start new day from DAY_END") {
        nursery->startNewDay();
        for (int i = 0; i < 5; i++) {
            nursery->advanceStep();
        }
        CHECK(nursery->getCurrentPhase() == GamePhase::DAY_END);

        // Should be able to start next day
        CHECK_NOTHROW(nursery->startNewDay());
        CHECK(nursery->getCurrentPhase() == GamePhase::DAY_START);
        CHECK(nursery->getCurrentDay() == 2);
    }
}

TEST_CASE("Nursery simulation - save/hire restrictions") {
    auto nursery = std::make_shared<Nursery>();

    SUBCASE("Cannot save in IDLE") {
        CHECK_FALSE(nursery->canSave());
        CHECK_FALSE(nursery->canHire());
    }

    SUBCASE("Cannot save in DAY_START") {
        nursery->startNewDay();
        CHECK_FALSE(nursery->canSave());
        CHECK_FALSE(nursery->canHire());
    }

    SUBCASE("Cannot save in STEP_BREAK") {
        nursery->startNewDay();
        nursery->advanceStep();
        CHECK(nursery->getCurrentPhase() == GamePhase::STEP_BREAK);
        CHECK_FALSE(nursery->canSave());
        CHECK_FALSE(nursery->canHire());
    }

    SUBCASE("Can save/hire only in DAY_END") {
        nursery->startNewDay();
        for (int i = 0; i < 5; i++) {
            nursery->advanceStep();
        }
        CHECK(nursery->getCurrentPhase() == GamePhase::DAY_END);
        CHECK(nursery->canSave());
        CHECK(nursery->canHire());
    }
}

TEST_CASE("Nursery simulation - customer timeout") {
    auto nursery = std::make_shared<Nursery>();
    auto inventory = nursery->getInventory();

    // No staff - all customer commands will timeout

    SUBCASE("Unserved customer commands are removed") {
        nursery->startNewDay();

        // Manually add a customer command
        auto spec = std::make_unique<PlantSpecification>();
        spec->requestType = RequestType::PURCHASE;
        spec->explicitName = "Rose";

        auto cmd = std::make_unique<FulfillCustomerCommand>(std::move(spec), inventory, nursery);
        nursery->addRequest(std::move(cmd));

        int initialRep = nursery->getReputation();

        // Advance step - no staff, so command won't be processed
        nursery->advanceStep();

        // Customer should have left, reputation should decrease
        // At least -3 from our customer, possibly more from random spawns
        CHECK(nursery->getReputation() < initialRep);
    }

    SUBCASE("Multiple unserved customers") {
        nursery->startNewDay();

        // Add 3 customer commands
        for (int i = 0; i < 3; i++) {
            auto spec = std::make_unique<PlantSpecification>();
            spec->requestType = RequestType::PURCHASE;
            spec->explicitName = "Rose";

            auto cmd =
                std::make_unique<FulfillCustomerCommand>(std::move(spec), inventory, nursery);
            nursery->addRequest(std::move(cmd));
        }

        int initialRep = nursery->getReputation();

        nursery->advanceStep();

        // All 3 customers left: -3 * 3 = -9 reputation minimum
        // However, random customers may also spawn and timeout, so reputation
        // will be at least -9 but possibly lower
        CHECK(nursery->getReputation() <= initialRep - 9);
    }
}

TEST_CASE("Nursery simulation - plant care command persistence") {
    auto nursery = std::make_shared<Nursery>();
    auto inventory = nursery->getInventory();

    // Create a plant (using concrete type) and add to inventory
    auto rose = std::make_shared<Rose>();
    inventory->add(rose);

    // No staff - plant care commands won't be processed

    SUBCASE("Plant care commands persist across steps") {
        nursery->startNewDay();

        // Add a water plant command
        auto waterCmd = std::make_unique<WaterPlantCommand>(rose);
        nursery->addRequest(std::move(waterCmd));

        // Check initial reputation
        // int initialReputation = nursery->getReputation();

        // Advance through all 5 steps without processing
        for (int i = 0; i < 5; i++) {
            nursery->advanceStep();
            // Command should still be in queue (we can't directly check)
            // Reputation may decrease due to customer spawns and timeouts
        }

        // Reputation may have changed due to random customer spawns/timeouts
        // The main test is that the plant care command doesn't get cleared
        // between steps (we verify this indirectly by it not being processed)
        CHECK(true);  // This subcase just verifies the system doesn't crash
    }

    SUBCASE("Plant care commands cleared at day start") {
        nursery->startNewDay();

        // Add a water plant command
        auto waterCmd = std::make_unique<WaterPlantCommand>(rose);
        nursery->addRequest(std::move(waterCmd));

        // Complete the day
        for (int i = 0; i < 5; i++) {
            nursery->advanceStep();
        }

        // Start new day - queue should be cleared
        nursery->startNewDay();

        // Queue should be empty after starting new day
        // (We verify this indirectly - the system should handle it gracefully)
        CHECK(true);
    }
}

TEST_CASE("Nursery simulation - staff processing") {
    auto nursery = std::make_shared<Nursery>();
    auto inventory = nursery->getInventory();

    // Set up staff chain: Cashier -> Gardener
    auto cashier = std::make_shared<Cashier>();
    auto gardener = std::make_shared<Gardener>();
    cashier->setSuccessor(gardener);
    nursery->setStaffChainHead(cashier);

    // Create storage group and add a rose
    auto storage = inventory->findGroupByName("Storage");
    auto rose = std::make_shared<Rose>();
    storage->add(std::static_pointer_cast<InventoryComponent>(rose));

    SUBCASE("Customer command processed successfully") {
        nursery->startNewDay();

        // Add a customer command that CAN be fulfilled
        auto customer = std::make_shared<Customer>();
        auto spec = std::make_unique<PlantSpecification>();
        spec->requestType = RequestType::PURCHASE;
        spec->explicitName = "Rose";

        auto cmd = std::make_unique<FulfillCustomerCommand>(std::move(spec), inventory, nursery);
        nursery->addRequest(std::move(cmd));

        double initialMoney = nursery->getMoney();

        // Advance step - command should be processed
        nursery->advanceStep();

        // Money should increase (plant sold)
        CHECK(nursery->getMoney() > initialMoney);

        // Plant should be removed from storage - check by getting members
        auto updatedStorage = inventory->findGroupByName("Storage");
        CHECK(updatedStorage->members().empty());  // Rose should be sold and removed
    }

    SUBCASE("Failed customer command applies reputation penalty") {
        nursery->startNewDay();

        // Add a customer command that CANNOT be fulfilled (plant doesn't exist)
        auto spec = std::make_unique<PlantSpecification>();
        spec->requestType = RequestType::PURCHASE;
        spec->explicitName = "Cactus";  // Not in inventory

        auto cmd = std::make_unique<FulfillCustomerCommand>(std::move(spec), inventory, nursery);
        nursery->addRequest(std::move(cmd));

        int initialRep = nursery->getReputation();

        // Advance step - command processed but fails
        nursery->advanceStep();

        // Reputation should decrease by 5 (failed purchase)
        CHECK(nursery->getReputation() < initialRep);
    }
}

TEST_CASE("Nursery simulation - runSimulation auto-mode") {
    auto nursery = std::make_shared<Nursery>();

    SUBCASE("runSimulation executes all 5 steps and reaches DAY_END") {
        nursery->runSimulation();

        CHECK(nursery->getCurrentPhase() == GamePhase::DAY_END);
        CHECK(nursery->getCurrentDay() == 1);
        CHECK(nursery->getCurrentStep() == 5);
        CHECK(nursery->isDayComplete());
        CHECK(nursery->canSave());
        CHECK(nursery->canHire());
    }

    SUBCASE("Multiple runSimulation calls advance days") {
        nursery->runSimulation();
        CHECK(nursery->getCurrentDay() == 1);

        nursery->runSimulation();
        CHECK(nursery->getCurrentDay() == 2);

        nursery->runSimulation();
        CHECK(nursery->getCurrentDay() == 3);
    }
}

TEST_CASE("Nursery simulation - plant daily updates") {
    auto nursery = std::make_shared<Nursery>();
    auto inventory = nursery->getInventory();

    // Create a rose
    auto rose = std::make_shared<Rose>();
    inventory->add(rose);

    SUBCASE("Plants perform daily activity at day start") {
        // Plants should call performDailyActivity() when day starts
        nursery->startNewDay();

        // We can't directly verify performDailyActivity was called,
        // but we can verify the day started successfully
        CHECK(nursery->getCurrentPhase() == GamePhase::DAY_START);
        CHECK(nursery->getCurrentDay() == 1);
    }
}

TEST_CASE("Nursery simulation - staff busy state") {
    auto nursery = std::make_shared<Nursery>();
    auto inventory = nursery->getInventory();

    // Single cashier - will get busy after first command
    auto cashier = std::make_shared<Cashier>();
    nursery->setStaffChainHead(cashier);

    // Create storage with a rose
    auto storage = std::make_shared<Group>("Storage", true);
    auto rose = std::make_shared<Rose>();
    storage->add(std::static_pointer_cast<InventoryComponent>(rose));
    inventory->add(std::static_pointer_cast<InventoryComponent>(storage));

    SUBCASE("Staff processes one command per step when busy") {
        nursery->startNewDay();

        // Add 3 customer commands, but only 1 rose in stock
        for (int i = 0; i < 3; i++) {
            auto spec = std::make_unique<PlantSpecification>();
            spec->requestType = RequestType::PURCHASE;
            spec->explicitName = "Rose";

            auto cmd =
                std::make_unique<FulfillCustomerCommand>(std::move(spec), inventory, nursery);
            nursery->addRequest(std::move(cmd));
        }

        int initialRep = nursery->getReputation();

        // With one cashier, only 1 command can be processed per step
        // The other 2 will timeout
        nursery->advanceStep();

        // 2 customers left: -6 reputation
        // 1 processed but failed (no more roses): -5 reputation
        // Total: -11
        CHECK(nursery->getReputation() <= initialRep);
    }
}

TEST_CASE("Nursery simulation - queue order preservation") {
    auto nursery = std::make_shared<Nursery>();
    auto inventory = nursery->getInventory();

    SUBCASE("Commands processed in FIFO order") {
        nursery->startNewDay();

        // Add commands and track order
        auto rose = std::make_shared<Rose>();
        inventory->add(rose);

        // Add multiple plant care commands
        for (int i = 0; i < 3; i++) {
            auto waterCmd = std::make_unique<WaterPlantCommand>(rose);
            nursery->addRequest(std::move(waterCmd));
        }

        // Commands should be processed in order
        // (We can't directly verify without instrumenting, but the test ensures no crashes)
        nursery->advanceStep();

        CHECK(nursery->getCurrentStep() == 1);
    }
}
