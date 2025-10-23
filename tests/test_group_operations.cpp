#include "../include/doctest.h"

#include <memory>

#include "../include/Components/Cactus.h"
#include "../include/Components/Group.h"
#include "../include/Components/Rose.h"
#include "../include/Patterns/State/PlantState.h"

/**
 * @file test_group_operations.cpp
 * @brief Comprehensive tests for Group composite operations
 * 
 * Tests cover:
 * - Price calculation (sum of children)
 * - Ownership management (owning vs non-owning groups)
 * - Auto-move ownership transfers
 * - Pruning expired references
 * - Edge cases and error handling
 */

TEST_SUITE("Group Operations") {
    
    TEST_CASE("Group::getPrice() sums children's prices") {
        auto garden = std::make_shared<Group>("Garden", true);
        
        auto rose1 = std::make_shared<Rose>();
        auto rose2 = std::make_shared<Rose>();
        auto cactus = std::make_shared<Cactus>();
        
        // Empty group has price 0
        CHECK(garden->getPrice() == 0.0);
        
        garden->add(rose1);
        double priceWithOneRose = garden->getPrice();
        CHECK(priceWithOneRose > 0.0);
        
        garden->add(rose2);
        double priceWithTwoRoses = garden->getPrice();
        CHECK(priceWithTwoRoses > priceWithOneRose);
        
        garden->add(cactus);
        double priceWithAll = garden->getPrice();
        CHECK(priceWithAll > priceWithTwoRoses);
        
        // Price should equal sum of individual prices
        double expectedTotal = rose1->getPrice() + rose2->getPrice() + cactus->getPrice();
        CHECK(garden->getPrice() == doctest::Approx(expectedTotal));
        
        // IMPORTANT: This is the correct way to get total value
        // Just call getPrice() on the Group - don't iterate and sum
    }
    
    TEST_CASE("Nested groups: price calculation cascades") {
        auto nursery = std::make_shared<Group>("Nursery", true);
        auto section1 = std::make_shared<Group>("Section 1", true);
        auto section2 = std::make_shared<Group>("Section 2", true);
        
        auto rose = std::make_shared<Rose>();
        auto cactus = std::make_shared<Cactus>();
        
        section1->add(rose);
        section2->add(cactus);
        nursery->add(section1);
        nursery->add(section2);
        
        // Nursery price should equal sum of all nested components
        double expectedTotal = rose->getPrice() + cactus->getPrice();
        CHECK(nursery->getPrice() == doctest::Approx(expectedTotal));
    }
    
    TEST_CASE("Owning group takes ownership") {
        auto owningGroup = std::make_shared<Group>("Owner", true);
        auto rose = std::make_shared<Rose>();
        
        CHECK(rose->getOwner().get() == nullptr);
        
        owningGroup->add(rose);
        
        CHECK(rose->getOwner().get() != nullptr);
        CHECK(rose->getOwner().get() == owningGroup.get());
    }
    
    TEST_CASE("Non-owning group does not take ownership") {
        auto nonOwningGroup = std::make_shared<Group>("Reference", false);
        auto rose = std::make_shared<Rose>();
        
        CHECK(rose->getOwner().get() == nullptr);
        
        nonOwningGroup->add(rose);
        
        // Should still have no owner
        CHECK(rose->getOwner().get() == nullptr);
    }
    
    TEST_CASE("Auto-move: transferring ownership between groups") {
        auto group1 = std::make_shared<Group>("Group 1", true);
        auto group2 = std::make_shared<Group>("Group 2", true);
        auto rose = std::make_shared<Rose>();
        
        // Add to first group
        group1->add(rose);
        CHECK(rose->getOwner().get() == group1.get());
        CHECK(group1->members().size() == 1);
        CHECK(group2->members().size() == 0);
        
        // Add to second group - should auto-move
        group2->add(rose);
        CHECK(rose->getOwner().get() == group2.get());
        CHECK(group1->members().size() == 0);  // Removed from group1
        CHECK(group2->members().size() == 1);  // Added to group2
    }
    
    TEST_CASE("Auto-move: ownership transfer updates both groups") {
        auto storage = std::make_shared<Group>("Storage", true);
        auto display = std::make_shared<Group>("Display", true);
        
        auto rose1 = std::make_shared<Rose>();
        auto rose2 = std::make_shared<Rose>();
        auto cactus = std::make_shared<Cactus>();
        
        storage->add(rose1);
        storage->add(rose2);
        storage->add(cactus);
        
        CHECK(storage->members().size() == 3);
        CHECK(display->members().size() == 0);
        
        // Move one plant to display
        display->add(rose1);
        
        CHECK(storage->members().size() == 2);
        CHECK(display->members().size() == 1);
        CHECK(rose1->getOwner().get() == display.get());
        
        // Move another plant
        display->add(cactus);
        
        CHECK(storage->members().size() == 1);
        CHECK(display->members().size() == 2);
    }
    
    TEST_CASE("Remove clears ownership") {
        auto group = std::make_shared<Group>("Group", true);
        auto rose = std::make_shared<Rose>();
        
        group->add(rose);
        CHECK(rose->getOwner().get() != nullptr);
        
        group->remove(rose);
        CHECK(rose->getOwner().get() == nullptr);
    }
    
    TEST_CASE("Non-owning group: price includes referenced components") {
        auto actualStorage = std::make_shared<Group>("Storage", true);
        auto viewGroup = std::make_shared<Group>("View", false);
        
        auto rose = std::make_shared<Rose>();
        auto cactus = std::make_shared<Cactus>();
        
        // Storage owns the plants
        actualStorage->add(rose);
        actualStorage->add(cactus);
        
        // View references them
        viewGroup->add(rose);
        viewGroup->add(cactus);
        
        // Both groups should report same total price
        CHECK(viewGroup->getPrice() == doctest::Approx(actualStorage->getPrice()));
    }
    
    TEST_CASE("pruneExpiredReferences removes dead weak_ptrs") {
        auto viewGroup = std::make_shared<Group>("View", false);
        
        {
            auto owningGroup = std::make_shared<Group>("Owner", true);
            auto rose = std::make_shared<Rose>();
            auto cactus = std::make_shared<Cactus>();
            
            owningGroup->add(rose);
            owningGroup->add(cactus);
            
            viewGroup->add(rose);
            viewGroup->add(cactus);
            
            CHECK(viewGroup->members().size() == 2);
        }
        // owningGroup goes out of scope, destroying rose and cactus
        
        // View group still has expired weak_ptrs, but members() filters them out
        CHECK(viewGroup->members().size() == 0);  // members() only returns valid refs
        
        // Prune to actually remove the expired weak_ptrs from internal storage
        viewGroup->pruneExpiredReferences();
        CHECK(viewGroup->members().size() == 0);
    }
    
    TEST_CASE("pruneExpiredReferences keeps valid references") {
        auto owningGroup = std::make_shared<Group>("Owner", true);
        auto viewGroup = std::make_shared<Group>("View", false);
        
        auto rose = std::make_shared<Rose>();
        auto cactus = std::make_shared<Cactus>();
        
        owningGroup->add(rose);
        owningGroup->add(cactus);
        
        viewGroup->add(rose);
        viewGroup->add(cactus);
        
        // Prune should not affect valid references
        viewGroup->pruneExpiredReferences();
        CHECK(viewGroup->members().size() == 2);
    }
    
    TEST_CASE("Complex ownership scenario") {
        auto nursery = std::make_shared<Group>("Nursery", true);
        auto plot1 = std::make_shared<Group>("Plot 1", true);
        auto plot2 = std::make_shared<Group>("Plot 2", true);
        auto allPlantsView = std::make_shared<Group>("All Plants", false);
        
        auto rose1 = std::make_shared<Rose>();
        auto rose2 = std::make_shared<Rose>();
        auto cactus = std::make_shared<Cactus>();
        
        // Initial setup
        plot1->add(rose1);
        plot1->add(rose2);
        plot2->add(cactus);
        
        nursery->add(plot1);
        nursery->add(plot2);
        
        // View references all plants
        allPlantsView->add(rose1);
        allPlantsView->add(rose2);
        allPlantsView->add(cactus);
        
        // Verify structure
        CHECK(nursery->members().size() == 2);  // 2 plots
        CHECK(plot1->members().size() == 2);
        CHECK(plot2->members().size() == 1);
        CHECK(allPlantsView->members().size() == 3);
        
        // Verify prices
        double expectedNurseryPrice = rose1->getPrice() + rose2->getPrice() + cactus->getPrice();
        CHECK(nursery->getPrice() == doctest::Approx(expectedNurseryPrice));
        CHECK(allPlantsView->getPrice() == doctest::Approx(expectedNurseryPrice));
        
        // Move a plant between plots
        plot2->add(rose1);  // Auto-move from plot1 to plot2
        
        CHECK(plot1->members().size() == 1);  // Only rose2 left
        CHECK(plot2->members().size() == 2);  // cactus + rose1
        CHECK(allPlantsView->members().size() == 3);  // Still sees all 3
        CHECK(nursery->getPrice() == doctest::Approx(expectedNurseryPrice));  // Total unchanged
    }
    
    TEST_CASE("Edge case: adding null component") {
        auto group = std::make_shared<Group>("Group", true);
        
        // Should not crash
        group->add(nullptr);
        CHECK(group->members().size() == 0);
    }
    
    TEST_CASE("Edge case: removing null component") {
        auto group = std::make_shared<Group>("Group", true);
        
        // Should not crash
        group->remove(nullptr);
        CHECK(group->members().size() == 0);
    }
    
    TEST_CASE("Edge case: removing non-existent component") {
        auto group = std::make_shared<Group>("Group", true);
        auto rose = std::make_shared<Rose>();
        auto cactus = std::make_shared<Cactus>();
        
        group->add(rose);
        
        // Try to remove something that's not in the group
        group->remove(cactus);
        
        CHECK(group->members().size() == 1);
        CHECK(rose->getOwner().get() != nullptr);
    }
    
    TEST_CASE("Mixed owning and non-owning groups") {
        auto storage = std::make_shared<Group>("Storage", true);
        auto summerView = std::make_shared<Group>("Summer Plants", false);
        auto expensiveView = std::make_shared<Group>("Expensive", false);
        
        auto rose1 = std::make_shared<Rose>();
        auto rose2 = std::make_shared<Rose>();
        auto cactus = std::make_shared<Cactus>();
        
        // Storage owns everything
        storage->add(rose1);
        storage->add(rose2);
        storage->add(cactus);
        
        // Views reference subsets
        summerView->add(rose1);
        summerView->add(rose2);
        
        expensiveView->add(rose1);
        
        // Verify ownership
        CHECK(rose1->getOwner().get() == storage.get());
        CHECK(rose2->getOwner().get() == storage.get());
        CHECK(cactus->getOwner().get() == storage.get());
        
        // Verify views
        CHECK(summerView->members().size() == 2);
        CHECK(expensiveView->members().size() == 1);
        
        // Prices
        CHECK(storage->getPrice() == doctest::Approx(
            rose1->getPrice() + rose2->getPrice() + cactus->getPrice()));
        CHECK(summerView->getPrice() == doctest::Approx(
            rose1->getPrice() + rose2->getPrice()));
        CHECK(expensiveView->getPrice() == doctest::Approx(rose1->getPrice()));
    }
}
