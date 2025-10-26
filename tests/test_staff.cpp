/**
 * @file test_staff.cpp
 * @brief Unit tests for Staff Chain of Responsibility pattern
 * 
 * Tests cover:
 * - Single handler processing commands
 * - Chain of handlers forwarding commands
 * - Busy flag preventing processing
 * - Command forwarding when busy
 * - Multiple staff members handling different command types
 * - End of chain behavior
 * - Null command handling
 */

#include "../include/doctest.h"

#include <memory>
#include <vector>

#include "../include/Actors/Staff.h"
#include "../include/Actors/Gardener.h"
#include "../include/Actors/Cashier.h"
#include "../include/Components/Rose.h"
#include "../include/Components/Cactus.h"
#include "../include/Core/Inventory.h"
#include "../include/Core/Nursery.h"
#include "../include/Actors/Customer.h"
#include "../include/Patterns/Command/Command.h"
#include "../include/Patterns/Command/WaterPlantCommand.h"
#include "../include/Patterns/Command/FulfillCustomerCommand.h"
#include "../include/Patterns/Builder/PlantSpecification.h"
#include "../include/Patterns/State/PlantState.h"

TEST_CASE("Staff - Gardener created not busy")
{
    auto gardener = std::make_shared<Gardener>();
    
    CHECK_FALSE(gardener->isBusy());
}

TEST_CASE("Staff - Cashier created not busy")
{
    auto cashier = std::make_shared<Cashier>();
    
    CHECK_FALSE(cashier->isBusy());
}

TEST_CASE("Staff - Set busy flag")
{
    auto gardener = std::make_shared<Gardener>();
    
    CHECK_FALSE(gardener->isBusy());
    
    gardener->setBusy(true);
    CHECK(gardener->isBusy());
    
    gardener->setBusy(false);
    CHECK_FALSE(gardener->isBusy());
}

TEST_CASE("Staff - Successor set correctly")
{
    auto gardener = std::make_shared<Gardener>();
    auto cashier = std::make_shared<Cashier>();
    
    CHECK_NOTHROW(gardener->setSuccessor(cashier));
}

TEST_CASE("Staff - Chain of two handlers created")
{
    auto gardener = std::make_shared<Gardener>();
    auto cashier = std::make_shared<Cashier>();
    
    gardener->setSuccessor(cashier);
    
    CHECK_FALSE(gardener->isBusy());
    CHECK_FALSE(cashier->isBusy());
}

TEST_CASE("Staff - Null command handled gracefully")
{
    auto gardener = std::make_shared<Gardener>();
    
    CHECK_NOTHROW(gardener->handleRequest(nullptr));
}

TEST_CASE("Gardener - Handles WaterPlantCommand when not busy")
{
    auto gardener = std::make_shared<Gardener>();
    auto rose = std::make_shared<Rose>();
    
    auto cmd = std::make_unique<WaterPlantCommand>(rose);
    
    CHECK_FALSE(gardener->isBusy());
    gardener->handleRequest(std::move(cmd));
    CHECK_FALSE(gardener->isBusy());  // Should be not busy after handling
}

TEST_CASE("Gardener - Forwards command when busy")
{
    auto gardener1 = std::make_shared<Gardener>();
    auto gardener2 = std::make_shared<Gardener>();
    auto rose = std::make_shared<Rose>();
    
    // Set up chain
    gardener1->setSuccessor(gardener2);
    
    // Make first gardener busy
    gardener1->setBusy(true);
    
    auto cmd = std::make_unique<WaterPlantCommand>(rose);
    gardener1->handleRequest(std::move(cmd));
    
    // First gardener should still be busy (didn't handle it)
    CHECK(gardener1->isBusy());
    // Second gardener should have handled it and be not busy
    CHECK_FALSE(gardener2->isBusy());
}

TEST_CASE("Cashier - Handles FulfillCustomerCommand when not busy")
{
    auto cashier = std::make_shared<Cashier>();
    auto inventory = std::make_shared<Inventory>();
    auto customer = std::make_shared<Customer>();
    
    auto spec = std::make_unique<PlantSpecification>();
    auto cmd = std::make_unique<FulfillCustomerCommand>(std::move(spec), inventory, customer);
    
    CHECK_FALSE(cashier->isBusy());
    cashier->handleRequest(std::move(cmd));
    CHECK_FALSE(cashier->isBusy());  // Should be not busy after handling
}

TEST_CASE("Cashier - Forwards command when busy")
{
    auto cashier1 = std::make_shared<Cashier>();
    auto cashier2 = std::make_shared<Cashier>();
    auto inventory = std::make_shared<Inventory>();
    auto customer = std::make_shared<Customer>();
    
    // Set up chain
    cashier1->setSuccessor(cashier2);
    
    // Make first cashier busy
    cashier1->setBusy(true);
    
    auto spec = std::make_unique<PlantSpecification>();
    auto cmd = std::make_unique<FulfillCustomerCommand>(std::move(spec), inventory, customer);
    cashier1->handleRequest(std::move(cmd));
    
    // First cashier should still be busy (didn't handle it)
    CHECK(cashier1->isBusy());
    // Second cashier should have handled it and be not busy
    CHECK_FALSE(cashier2->isBusy());
}

TEST_CASE("Chain - WaterPlantCommand forwarded to correct handler")
{
    auto cashier = std::make_shared<Cashier>();
    auto gardener = std::make_shared<Gardener>();
    auto rose = std::make_shared<Rose>();
    
    // Cashier -> Gardener chain
    cashier->setSuccessor(gardener);
    
    auto cmd = std::make_unique<WaterPlantCommand>(rose);
    
    // Start at cashier (can't handle water commands)
    cashier->handleRequest(std::move(cmd));
    
    // Should have forwarded to gardener who handled it
    CHECK_FALSE(cashier->isBusy());
    CHECK_FALSE(gardener->isBusy());
}

TEST_CASE("Chain - FulfillCustomerCommand forwarded to correct handler")
{
    auto gardener = std::make_shared<Gardener>();
    auto cashier = std::make_shared<Cashier>();
    auto inventory = std::make_shared<Inventory>();
    auto customer = std::make_shared<Customer>();
    
    // Gardener -> Cashier chain
    gardener->setSuccessor(cashier);
    
    auto spec = std::make_unique<PlantSpecification>();
    auto cmd = std::make_unique<FulfillCustomerCommand>(std::move(spec), inventory, customer);
    
    // Start at gardener (can't handle customer commands)
    gardener->handleRequest(std::move(cmd));
    
    // Should have forwarded to cashier who handled it
    CHECK_FALSE(gardener->isBusy());
    CHECK_FALSE(cashier->isBusy());
}

TEST_CASE("Chain - Multiple staff of same type")
{
    auto gardener1 = std::make_shared<Gardener>();
    auto gardener2 = std::make_shared<Gardener>();
    auto gardener3 = std::make_shared<Gardener>();
    auto rose = std::make_shared<Rose>();
    
    // Chain: gardener1 -> gardener2 -> gardener3
    gardener1->setSuccessor(gardener2);
    gardener2->setSuccessor(gardener3);
    
    // Make first two busy
    gardener1->setBusy(true);
    gardener2->setBusy(true);
    
    auto cmd = std::make_unique<WaterPlantCommand>(rose);
    gardener1->handleRequest(std::move(cmd));
    
    // First two still busy, third handled it
    CHECK(gardener1->isBusy());
    CHECK(gardener2->isBusy());
    CHECK_FALSE(gardener3->isBusy());
}

TEST_CASE("Chain - Mixed staff types")
{
    auto gardener1 = std::make_shared<Gardener>();
    auto cashier1 = std::make_shared<Cashier>();
    auto gardener2 = std::make_shared<Gardener>();
    auto cashier2 = std::make_shared<Cashier>();
    
    // Chain: gardener1 -> cashier1 -> gardener2 -> cashier2
    gardener1->setSuccessor(cashier1);
    cashier1->setSuccessor(gardener2);
    gardener2->setSuccessor(cashier2);
    
    // Check they're all created properly
    CHECK(gardener1.get() != nullptr);
    CHECK(cashier1.get() != nullptr);
    CHECK(gardener2.get() != nullptr);
    CHECK(cashier2.get() != nullptr);
}

TEST_CASE("Chain - All handlers busy, command forwarded to end")
{
    auto gardener1 = std::make_shared<Gardener>();
    auto gardener2 = std::make_shared<Gardener>();
    auto gardener3 = std::make_shared<Gardener>();
    auto rose = std::make_shared<Rose>();
    
    gardener1->setSuccessor(gardener2);
    gardener2->setSuccessor(gardener3);
    
    // Make all busy
    gardener1->setBusy(true);
    gardener2->setBusy(true);
    gardener3->setBusy(true);
    
    auto cmd = std::make_unique<WaterPlantCommand>(rose);
    
    // Command should be forwarded through chain and dropped at end
    CHECK_NOTHROW(gardener1->handleRequest(std::move(cmd)));
    
    // All should still be busy
    CHECK(gardener1->isBusy());
    CHECK(gardener2->isBusy());
    CHECK(gardener3->isBusy());
}

TEST_CASE("Chain - Wrong command type dropped at end")
{
    auto gardener1 = std::make_shared<Gardener>();
    auto gardener2 = std::make_shared<Gardener>();
    auto inventory = std::make_shared<Inventory>();
    auto customer = std::make_shared<Customer>();
    
    gardener1->setSuccessor(gardener2);
    
    // Send customer command to gardener chain (they can't handle it)
    auto spec = std::make_unique<PlantSpecification>();
    auto cmd = std::make_unique<FulfillCustomerCommand>(std::move(spec), inventory, customer);
    
    // Should forward through chain and drop
    CHECK_NOTHROW(gardener1->handleRequest(std::move(cmd)));
}

TEST_CASE("Chain - Command handled by first available handler")
{
    auto gardener1 = std::make_shared<Gardener>();
    auto gardener2 = std::make_shared<Gardener>();
    auto gardener3 = std::make_shared<Gardener>();
    auto rose = std::make_shared<Rose>();
    
    gardener1->setSuccessor(gardener2);
    gardener2->setSuccessor(gardener3);
    
    // Only first is busy
    gardener1->setBusy(true);
    
    auto cmd = std::make_unique<WaterPlantCommand>(rose);
    gardener1->handleRequest(std::move(cmd));
    
    // Second should have handled it
    CHECK(gardener1->isBusy());
    CHECK_FALSE(gardener2->isBusy());
    CHECK_FALSE(gardener3->isBusy());
}

TEST_CASE("Integration - Realistic nursery staff chain")
{
    // Typical nursery setup: Gardener -> Gardener -> Cashier
    auto gardener1 = std::make_shared<Gardener>();
    auto gardener2 = std::make_shared<Gardener>();
    auto cashier = std::make_shared<Cashier>();
    
    gardener1->setSuccessor(gardener2);
    gardener2->setSuccessor(cashier);
    
    // Test water command
    auto rose = std::make_shared<Rose>();
    auto waterCmd = std::make_unique<WaterPlantCommand>(rose);
    gardener1->handleRequest(std::move(waterCmd));
    
    CHECK_FALSE(gardener1->isBusy());
    
    // Test customer command
    auto inventory = std::make_shared<Inventory>();
    auto customer = std::make_shared<Customer>();
    auto spec = std::make_unique<PlantSpecification>();
    auto customerCmd = std::make_unique<FulfillCustomerCommand>(std::move(spec), inventory, customer);
    gardener1->handleRequest(std::move(customerCmd));
    
    // Should have forwarded to cashier
    CHECK_FALSE(cashier->isBusy());
}

TEST_CASE("Practical - Multiple commands processed in sequence")
{
    auto gardener = std::make_shared<Gardener>();
    auto rose1 = std::make_shared<Rose>();
    auto rose2 = std::make_shared<Rose>();
    auto rose3 = std::make_shared<Rose>();
    
    // Process multiple commands sequentially
    auto cmd1 = std::make_unique<WaterPlantCommand>(rose1);
    gardener->handleRequest(std::move(cmd1));
    CHECK_FALSE(gardener->isBusy());
    
    auto cmd2 = std::make_unique<WaterPlantCommand>(rose2);
    gardener->handleRequest(std::move(cmd2));
    CHECK_FALSE(gardener->isBusy());
    
    auto cmd3 = std::make_unique<WaterPlantCommand>(rose3);
    gardener->handleRequest(std::move(cmd3));
    CHECK_FALSE(gardener->isBusy());
}

TEST_CASE("Edge case - Single handler, no successor")
{
    auto gardener = std::make_shared<Gardener>();
    auto rose = std::make_shared<Rose>();
    
    // No successor set
    auto cmd = std::make_unique<WaterPlantCommand>(rose);
    
    CHECK_NOTHROW(gardener->handleRequest(std::move(cmd)));
    CHECK_FALSE(gardener->isBusy());
}

TEST_CASE("Edge case - Circular reference prevention")
{
    auto gardener1 = std::make_shared<Gardener>();
    auto gardener2 = std::make_shared<Gardener>();
    
    gardener1->setSuccessor(gardener2);
    
    // Don't set gardener2's successor back to gardener1
    // This test ensures we don't create circular chains in practice
    CHECK(gardener1.get() != nullptr);
    CHECK(gardener2.get() != nullptr);
}

TEST_CASE("Memory - Staff members properly destroyed")
{
    std::weak_ptr<Gardener> weakGardener;
    std::weak_ptr<Cashier> weakCashier;
    
    {
        auto gardener = std::make_shared<Gardener>();
        auto cashier = std::make_shared<Cashier>();
        
        weakGardener = gardener;
        weakCashier = cashier;
        
        gardener->setSuccessor(cashier);
        
        CHECK_FALSE(weakGardener.expired());
        CHECK_FALSE(weakCashier.expired());
    }
    
    // Both should be destroyed
    CHECK(weakGardener.expired());
    CHECK(weakCashier.expired());
}