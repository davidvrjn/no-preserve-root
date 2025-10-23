#include "../include/doctest.h"

#include <memory>
#include <string>

#include "../include/Components/Cactus.h"
#include "../include/Components/Group.h"
#include "../include/Components/Rose.h"
#include "../include/Patterns/Iterator/CompositeIterator.h"
#include "../include/Patterns/Iterator/FilteredTraversal.h"
#include "../include/Patterns/Iterator/Iterator.h"
#include "../include/Patterns/Iterator/LevelOrderTraversal.h"
#include "../include/Patterns/Iterator/PreOrderTraversal.h"
#include "../include/Patterns/State/PlantState.h"

/**
 * @file test_filtered_iterators.cpp
 * @brief Demonstrates how to create custom filtered iterators for specific use cases
 * 
 * This test suite shows real-world examples of filtering the inventory:
 * - Finding only specific plant types (Roses, Cacti)
 * - Excluding composite Groups to get only leaf nodes
 * - Using different traversal strategies with filters
 * - Handling empty results when no items match the filter
 * 
 * These examples serve as a guide for the team to create their own custom iterators
 * for specific business needs (e.g., seasonal plants, plants by price range,
 * plants needing water, etc.)
 * 
 * To create a custom filter:
 * 1. Define a predicate lambda that returns true for components you want to include
 * 2. Wrap your base strategy (PreOrder, LevelOrder, etc.) with FilteredTraversal
 * 3. Pass the filter predicate to FilteredTraversal
 * 4. Use the filtered iterator just like any other iterator
 */

TEST_SUITE("Filtered Iterator Examples") {
    
    TEST_CASE("Filter 1: Only Plants (exclude Groups)") {
        // Create a hierarchy with nested groups
        auto nursery = std::make_shared<Group>("Nursery", true);
        auto indoorSection = std::make_shared<Group>("Indoor Section", true);
        auto outdoorSection = std::make_shared<Group>("Outdoor Section", true);
        
        auto rose = std::make_shared<Rose>();
        auto cactus = std::make_shared<Cactus>();
        auto rose2 = std::make_shared<Rose>();
        
        indoorSection->add(cactus);
        outdoorSection->add(rose);
        outdoorSection->add(rose2);
        nursery->add(indoorSection);
        nursery->add(outdoorSection);
        
        // Create a filter that only returns Plants (not Groups)
        // This checks if the component can be cast to Rose or Cactus
        auto plantsOnlyFilter = [](const std::shared_ptr<InventoryComponent>& comp) {
            return std::dynamic_pointer_cast<Rose>(comp) != nullptr ||
                   std::dynamic_pointer_cast<Cactus>(comp) != nullptr;
        };
        
        auto filteredStrategy = std::make_unique<FilteredTraversal>(
            std::make_unique<PreOrderTraversal>(),
            plantsOnlyFilter
        );
        
        auto iterator = nursery->createIterator(std::move(filteredStrategy));
        
        // Should only get the 3 plants, not the 3 groups
        int plantCount = 0;
        while (iterator->hasNext()) {
            auto component = iterator->next();
            // Verify each item is either a Rose or Cactus
            bool isPlant = std::dynamic_pointer_cast<Rose>(component) != nullptr ||
                          std::dynamic_pointer_cast<Cactus>(component) != nullptr;
            CHECK(isPlant);
            plantCount++;
        }
        
        CHECK(plantCount == 3);  // Only the 3 plants, not the groups
    }
    
    TEST_CASE("Filter 2: Only Roses") {
        auto garden = std::make_shared<Group>("Garden", true);
        
        auto rose1 = std::make_shared<Rose>();
        auto rose2 = std::make_shared<Rose>();
        auto cactus = std::make_shared<Cactus>();
        auto cactus2 = std::make_shared<Cactus>();
        
        garden->add(rose1);
        garden->add(cactus);
        garden->add(rose2);
        garden->add(cactus2);
        
        // Filter for only Rose instances
        auto rosesOnlyFilter = [](const std::shared_ptr<InventoryComponent>& comp) {
            return std::dynamic_pointer_cast<Rose>(comp) != nullptr;
        };
        
        auto filteredStrategy = std::make_unique<FilteredTraversal>(
            std::make_unique<PreOrderTraversal>(),
            rosesOnlyFilter
        );
        
        auto iterator = garden->createIterator(std::move(filteredStrategy));
        
        // Should only find the 2 roses
        int roseCount = 0;
        while (iterator->hasNext()) {
            auto component = iterator->next();
            auto rose = std::dynamic_pointer_cast<Rose>(component);
            CHECK(rose.get() != nullptr);
            roseCount++;
        }
        
        CHECK(roseCount == 2);
    }
    
    TEST_CASE("Filter 3: Only Cacti") {
        auto garden = std::make_shared<Group>("Garden", true);
        
        auto rose = std::make_shared<Rose>();
        auto cactus1 = std::make_shared<Cactus>();
        auto cactus2 = std::make_shared<Cactus>();
        auto cactus3 = std::make_shared<Cactus>();
        
        garden->add(cactus1);
        garden->add(rose);
        garden->add(cactus2);
        garden->add(cactus3);
        
        // Filter for only Cactus instances
        auto cactiOnlyFilter = [](const std::shared_ptr<InventoryComponent>& comp) {
            return std::dynamic_pointer_cast<Cactus>(comp) != nullptr;
        };
        
        auto filteredStrategy = std::make_unique<FilteredTraversal>(
            std::make_unique<PreOrderTraversal>(),
            cactiOnlyFilter
        );
        
        auto iterator = garden->createIterator(std::move(filteredStrategy));
        
        // Should find the 3 cacti
        int cactusCount = 0;
        while (iterator->hasNext()) {
            auto component = iterator->next();
            auto cactus = std::dynamic_pointer_cast<Cactus>(component);
            CHECK(cactus.get() != nullptr);
            cactusCount++;
        }
        
        CHECK(cactusCount == 3);
    }
    
    TEST_CASE("Filter 4: Works with Different Base Strategies") {
        // This test shows that FilteredTraversal works with ANY base strategy
        auto hierarchy = std::make_shared<Group>("Root", true);
        auto level1 = std::make_shared<Group>("Level 1", true);
        
        auto rose1 = std::make_shared<Rose>();
        auto rose2 = std::make_shared<Rose>();
        auto cactus = std::make_shared<Cactus>();
        
        level1->add(rose1);
        hierarchy->add(level1);
        hierarchy->add(rose2);
        hierarchy->add(cactus);
        
        auto rosesFilter = [](const std::shared_ptr<InventoryComponent>& comp) {
            return std::dynamic_pointer_cast<Rose>(comp) != nullptr;
        };
        
        // Test with PreOrder traversal
        auto preOrderStrategy = std::make_unique<FilteredTraversal>(
            std::make_unique<PreOrderTraversal>(),
            rosesFilter
        );
        auto preOrderIter = hierarchy->createIterator(std::move(preOrderStrategy));
        
        int preOrderCount = 0;
        while (preOrderIter->hasNext()) {
            preOrderIter->next();
            preOrderCount++;
        }
        CHECK(preOrderCount == 2);
        
        // Test with LevelOrder traversal
        auto levelOrderStrategy = std::make_unique<FilteredTraversal>(
            std::make_unique<LevelOrderTraversal>(),
            rosesFilter
        );
        auto levelOrderIter = hierarchy->createIterator(std::move(levelOrderStrategy));
        
        int levelOrderCount = 0;
        while (levelOrderIter->hasNext()) {
            levelOrderIter->next();
            levelOrderCount++;
        }
        CHECK(levelOrderCount == 2);  // Same count, potentially different order
    }
    
    TEST_CASE("Filter 5: Empty Results When Nothing Matches") {
        auto garden = std::make_shared<Group>("Garden", true);
        
        // Only add roses
        auto rose1 = std::make_shared<Rose>();
        auto rose2 = std::make_shared<Rose>();
        
        garden->add(rose1);
        garden->add(rose2);
        
        // Filter for cacti (none exist)
        auto cactiFilter = [](const std::shared_ptr<InventoryComponent>& comp) {
            return std::dynamic_pointer_cast<Cactus>(comp) != nullptr;
        };
        
        auto filteredStrategy = std::make_unique<FilteredTraversal>(
            std::make_unique<PreOrderTraversal>(),
            cactiFilter
        );
        
        auto iterator = garden->createIterator(std::move(filteredStrategy));
        
        // Should have no results
        CHECK_FALSE(iterator->hasNext());
    }
    
    TEST_CASE("Practical Example: Calculate Total Value of Roses Only") {
        auto inventory = std::make_shared<Group>("Inventory", true);
        
        auto rose1 = std::make_shared<Rose>();
        auto rose2 = std::make_shared<Rose>();
        auto rose3 = std::make_shared<Rose>();
        auto cactus1 = std::make_shared<Cactus>();
        auto cactus2 = std::make_shared<Cactus>();
        
        inventory->add(rose1);
        inventory->add(cactus1);
        inventory->add(rose2);
        inventory->add(rose3);
        inventory->add(cactus2);
        
        // Create a filtered iterator for roses only
        auto rosesFilter = [](const std::shared_ptr<InventoryComponent>& comp) {
            return std::dynamic_pointer_cast<Rose>(comp) != nullptr;
        };
        
        auto filteredStrategy = std::make_unique<FilteredTraversal>(
            std::make_unique<PreOrderTraversal>(),
            rosesFilter
        );
        
        auto iterator = inventory->createIterator(std::move(filteredStrategy));
        
        // Calculate total value of all roses
        double totalRoseValue = 0.0;
        int roseCount = 0;
        while (iterator->hasNext()) {
            auto component = iterator->next();
            totalRoseValue += component->getPrice();
            roseCount++;
        }
        
        CHECK(roseCount == 3);  // Should have found all 3 roses
        CHECK(totalRoseValue > 0.0);  // Should have some total value
    }
    
    TEST_CASE("Nested Groups with Filtering") {
        // Create a more complex hierarchy
        auto store = std::make_shared<Group>("Store", true);
        auto section1 = std::make_shared<Group>("Section 1", true);
        auto section2 = std::make_shared<Group>("Section 2", true);
        auto subsection = std::make_shared<Group>("Subsection", true);
        
        auto rose1 = std::make_shared<Rose>();
        auto cactus1 = std::make_shared<Cactus>();
        auto rose2 = std::make_shared<Rose>();
        auto cactus2 = std::make_shared<Cactus>();
        auto rose3 = std::make_shared<Rose>();
        
        subsection->add(rose3);
        subsection->add(cactus2);
        
        section1->add(rose1);
        section1->add(cactus1);
        
        section2->add(subsection);
        section2->add(rose2);
        
        store->add(section1);
        store->add(section2);
        
        // Filter for only cacti across the entire hierarchy
        auto cactiFilter = [](const std::shared_ptr<InventoryComponent>& comp) {
            return std::dynamic_pointer_cast<Cactus>(comp) != nullptr;
        };
        
        auto filteredStrategy = std::make_unique<FilteredTraversal>(
            std::make_unique<PreOrderTraversal>(),
            cactiFilter
        );
        
        auto iterator = store->createIterator(std::move(filteredStrategy));
        
        // Should find both cacti regardless of nesting level
        int cactusCount = 0;
        while (iterator->hasNext()) {
            auto component = iterator->next();
            CHECK(std::dynamic_pointer_cast<Cactus>(component).get() != nullptr);
            cactusCount++;
        }
        
        CHECK(cactusCount == 2);  // cactus1 and cactus2
    }
}
