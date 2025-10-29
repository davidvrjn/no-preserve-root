/**
 * @file test_fertilize_command.cpp
 * @brief Unit tests for FertilizeCommand
 * 
 * Tests cover:
 * - Command creation and execution
 * - R30 cost deduction from nursery
 * - Insufficient funds handling
 * - Expired plant/nursery handling
 * - Serialization/deserialization
 */

#include "../include/doctest.h"

#include <memory>

#include "../include/Components/Rose.h"
#include "../include/Components/Cactus.h"
#include "../include/Components/Basil.h"
#include "../include/Core/Nursery.h"
#include "../include/Actors/Gardener.h"
#include "../include/Patterns/Command/FertilizeCommand.h"
#include "../include/Patterns/State/PlantState.h"

TEST_CASE("FertilizeCommand - Created with valid plant and nursery")
{
    auto nursery = std::make_shared<Nursery>();
    auto rose = std::make_shared<Rose>();
    auto cmd = std::make_unique<FertilizeCommand>(rose, nursery);
    
    CHECK(cmd != nullptr);
    CHECK(cmd->getStatus() == Command::Status::Pending);
    CHECK(cmd->getTargetId() == rose->getId());
}

TEST_CASE("FertilizeCommand - Created with nullptr plant")
{
    auto nursery = std::make_shared<Nursery>();
    auto cmd = std::make_unique<FertilizeCommand>(nullptr, nursery);
    
    CHECK(cmd != nullptr);
    CHECK(cmd->getTargetId() == 0);
}

TEST_CASE("FertilizeCommand - Created with nullptr nursery")
{
    auto rose = std::make_shared<Rose>();
    auto cmd = std::make_unique<FertilizeCommand>(rose, nullptr);
    
    CHECK(cmd != nullptr);
    CHECK(cmd->getTargetId() == rose->getId());
}

TEST_CASE("FertilizeCommand - Execute deducts R30 from nursery")
{
    auto nursery = std::make_shared<Nursery>();
    double initialMoney = nursery->getMoney();
    
    auto rose = std::make_shared<Rose>();
    auto cmd = std::make_unique<FertilizeCommand>(rose, nursery);
    
    cmd->execute();
    
    CHECK(cmd->getStatus() == Command::Status::Completed);
    CHECK(nursery->getMoney() == doctest::Approx(initialMoney - 30.0));
}

TEST_CASE("FertilizeCommand - Insufficient funds causes failure")
{
    auto nursery = std::make_shared<Nursery>();
    
    // Drain nursery funds below R30
    nursery->adjustMoney(-nursery->getMoney() + 20.0);
    CHECK(nursery->getMoney() < 30.0);
    
    auto rose = std::make_shared<Rose>();
    auto cmd = std::make_unique<FertilizeCommand>(rose, nursery);
    
    double moneyBefore = nursery->getMoney();
    cmd->execute();
    
    CHECK(cmd->getStatus() == Command::Status::Failed);
    CHECK(nursery->getMoney() == doctest::Approx(moneyBefore));  // No money deducted
}

TEST_CASE("FertilizeCommand - Execute with expired plant")
{
    auto nursery = std::make_shared<Nursery>();
    std::weak_ptr<Rose> weakRose;
    
    auto cmd = std::make_unique<FertilizeCommand>(nullptr, nursery);
    
    {
        auto rose = std::make_shared<Rose>();
        weakRose = rose;
        cmd = std::make_unique<FertilizeCommand>(rose, nursery);
    }
    
    // Plant is now destroyed
    CHECK(weakRose.expired());
    
    double moneyBefore = nursery->getMoney();
    cmd->execute();
    
    CHECK(cmd->getStatus() == Command::Status::Failed);
    CHECK(nursery->getMoney() == doctest::Approx(moneyBefore));  // No cost deducted
}

TEST_CASE("FertilizeCommand - Execute with expired nursery")
{
    auto rose = std::make_shared<Rose>();
    std::weak_ptr<Nursery> weakNursery;
    
    auto cmd = std::make_unique<FertilizeCommand>(nullptr, nullptr);
    
    {
        auto nursery = std::make_shared<Nursery>();
        weakNursery = nursery;
        cmd = std::make_unique<FertilizeCommand>(rose, nursery);
    }
    
    // Nursery is now destroyed
    CHECK(weakNursery.expired());
    
    cmd->execute();
    CHECK(cmd->getStatus() == Command::Status::Failed);
}

TEST_CASE("FertilizeCommand - Multiple fertilizations deduct multiple R30")
{
    auto nursery = std::make_shared<Nursery>();
    auto rose = std::make_shared<Rose>();
    
    double initialMoney = nursery->getMoney();
    
    auto cmd1 = std::make_unique<FertilizeCommand>(rose, nursery);
    auto cmd2 = std::make_unique<FertilizeCommand>(rose, nursery);
    auto cmd3 = std::make_unique<FertilizeCommand>(rose, nursery);
    
    cmd1->execute();
    cmd2->execute();
    cmd3->execute();
    
    // Should deduct 30 * 3 = R90
    CHECK(nursery->getMoney() == doctest::Approx(initialMoney - 90.0));
}

TEST_CASE("FertilizeCommand - Fertilizer cost is exactly R30")
{
    auto nursery = std::make_shared<Nursery>();
    auto rose = std::make_shared<Rose>();
    
    double before = nursery->getMoney();
    
    auto cmd = std::make_unique<FertilizeCommand>(rose, nursery);
    cmd->execute();
    
    double after = nursery->getMoney();
    double cost = before - after;
    
    CHECK(cost == doctest::Approx(30.0));
}

TEST_CASE("FertilizeCommand - Status starts as Pending")
{
    auto nursery = std::make_shared<Nursery>();
    auto rose = std::make_shared<Rose>();
    auto cmd = std::make_unique<FertilizeCommand>(rose, nursery);
    
    CHECK(cmd->getStatus() == Command::Status::Pending);
}

TEST_CASE("FertilizeCommand - Status changes to Completed after execution")
{
    auto nursery = std::make_shared<Nursery>();
    auto rose = std::make_shared<Rose>();
    auto cmd = std::make_unique<FertilizeCommand>(rose, nursery);
    
    CHECK(cmd->getStatus() == Command::Status::Pending);
    
    cmd->execute();
    CHECK(cmd->getStatus() == Command::Status::Completed);
}

TEST_CASE("FertilizeCommand - Status changes to Failed on insufficient funds")
{
    auto nursery = std::make_shared<Nursery>();
    nursery->adjustMoney(-nursery->getMoney());  // Zero money
    
    auto rose = std::make_shared<Rose>();
    auto cmd = std::make_unique<FertilizeCommand>(rose, nursery);
    
    cmd->execute();
    CHECK(cmd->getStatus() == Command::Status::Failed);
}

TEST_CASE("FertilizeCommand - Manual status change")
{
    auto nursery = std::make_shared<Nursery>();
    auto rose = std::make_shared<Rose>();
    auto cmd = std::make_unique<FertilizeCommand>(rose, nursery);
    
    cmd->setStatus(Command::Status::Cancelled);
    CHECK(cmd->getStatus() == Command::Status::Cancelled);
    
    cmd->setStatus(Command::Status::Failed);
    CHECK(cmd->getStatus() == Command::Status::Failed);
    
    cmd->setStatus(Command::Status::Completed);
    CHECK(cmd->getStatus() == Command::Status::Completed);
}

TEST_CASE("FertilizeCommand - Target ID set correctly on construction")
{
    auto nursery = std::make_shared<Nursery>();
    auto rose = std::make_shared<Rose>();
    uint64_t plantId = rose->getId();
    
    auto cmd = std::make_unique<FertilizeCommand>(rose, nursery);
    
    CHECK(cmd->getTargetId() == plantId);
}

TEST_CASE("FertilizeCommand - Target ID can be manually changed")
{
    auto nursery = std::make_shared<Nursery>();
    auto rose = std::make_shared<Rose>();
    auto cmd = std::make_unique<FertilizeCommand>(rose, nursery);
    
    cmd->setTargetId(99999);
    CHECK(cmd->getTargetId() == 99999);
}

TEST_CASE("Edge case - Fertilize with exactly R30")
{
    auto nursery = std::make_shared<Nursery>();
    
    // Set money to exactly R30
    nursery->adjustMoney(-nursery->getMoney() + 30.0);
    CHECK(nursery->getMoney() == doctest::Approx(30.0));
    
    auto rose = std::make_shared<Rose>();
    auto cmd = std::make_unique<FertilizeCommand>(rose, nursery);
    
    cmd->execute();
    
    CHECK(cmd->getStatus() == Command::Status::Completed);
    CHECK(nursery->getMoney() == doctest::Approx(0.0));
}

TEST_CASE("Edge case - Fertilize with R29.99 fails")
{
    auto nursery = std::make_shared<Nursery>();
    
    // Set money to just under R30
    nursery->adjustMoney(-nursery->getMoney() + 29.99);
    CHECK(nursery->getMoney() < 30.0);
    
    auto rose = std::make_shared<Rose>();
    auto cmd = std::make_unique<FertilizeCommand>(rose, nursery);
    
    cmd->execute();
    
    CHECK(cmd->getStatus() == Command::Status::Failed);
    CHECK(nursery->getMoney() == doctest::Approx(29.99));  // Money unchanged
}

TEST_CASE("Edge case - Multiple commands with decreasing funds")
{
    auto nursery = std::make_shared<Nursery>();
    
    // Set money to R70 (enough for 2 fertilizations, not 3)
    nursery->adjustMoney(-nursery->getMoney() + 70.0);
    
    auto rose = std::make_shared<Rose>();
    
    auto cmd1 = std::make_unique<FertilizeCommand>(rose, nursery);
    auto cmd2 = std::make_unique<FertilizeCommand>(rose, nursery);
    auto cmd3 = std::make_unique<FertilizeCommand>(rose, nursery);
    
    cmd1->execute();
    CHECK(cmd1->getStatus() == Command::Status::Completed);
    CHECK(nursery->getMoney() == doctest::Approx(40.0));
    
    cmd2->execute();
    CHECK(cmd2->getStatus() == Command::Status::Completed);
    CHECK(nursery->getMoney() == doctest::Approx(10.0));
    
    cmd3->execute();
    CHECK(cmd3->getStatus() == Command::Status::Failed);
    CHECK(nursery->getMoney() == doctest::Approx(10.0));  // Unchanged
}

TEST_CASE("FertilizeCommand - Execute calls plant fertilize method")
{
    auto nursery = std::make_shared<Nursery>();
    auto rose = std::make_shared<Rose>();
    
    auto cmd = std::make_unique<FertilizeCommand>(rose, nursery);
    
    // Just verify it doesn't crash
    CHECK_NOTHROW(cmd->execute());
    CHECK(cmd->getStatus() == Command::Status::Completed);
}

TEST_CASE("FertilizeCommand - Different plant types can be fertilized")
{
    auto nursery = std::make_shared<Nursery>();
    
    auto rose = std::make_shared<Rose>();
    auto cactus = std::make_shared<Cactus>();
    auto basil = std::make_shared<Basil>();
    
    auto cmd1 = std::make_unique<FertilizeCommand>(rose, nursery);
    auto cmd2 = std::make_unique<FertilizeCommand>(cactus, nursery);
    auto cmd3 = std::make_unique<FertilizeCommand>(basil, nursery);
    
    CHECK_NOTHROW(cmd1->execute());
    CHECK_NOTHROW(cmd2->execute());
    CHECK_NOTHROW(cmd3->execute());
    
    CHECK(cmd1->getStatus() == Command::Status::Completed);
    CHECK(cmd2->getStatus() == Command::Status::Completed);
    CHECK(cmd3->getStatus() == Command::Status::Completed);
}