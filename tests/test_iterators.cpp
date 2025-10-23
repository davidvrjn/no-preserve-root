/**
 * @file test_iterators.cpp
 * @brief Unit tests for Iterator pattern implementation
 * 
 * Tests cover:
 * - CompositeIterator with different traversal strategies
 * - PreOrderTraversal (depth-first, root-first)
 * - LevelOrderTraversal (breadth-first, level-by-level)
 * - Leaf node iteration (single plant)
 * - Group iteration (nested structures)
 */

#include "../include/doctest.h"

#include <memory>
#include <vector>

#include "../include/Components/Group.h"
#include "../include/Components/Rose.h"
#include "../include/Components/Cactus.h"
#include "../include/Components/Basil.h"
#include "../include/Patterns/Iterator/Iterator.h"
#include "../include/Patterns/Iterator/CompositeIterator.h"
#include "../include/Patterns/Iterator/PreOrderTraversal.h"
#include "../include/Patterns/Iterator/LevelOrderTraversal.h"
#include "../include/Patterns/State/PlantState.h"  // Needed for complete type

TEST_CASE("Leaf iterator - Single plant returns itself once") {
    auto rose = std::make_shared<Rose>();
    auto iter = rose->createIterator();
    
    REQUIRE(iter != nullptr);
    CHECK(iter->hasNext());
    
    auto first = iter->next();
    CHECK(first.get() == rose.get());
    CHECK(first->getName() == "Rose");
    
    CHECK_FALSE(iter->hasNext());
    CHECK(iter->next().get() == nullptr);
}

TEST_CASE("Leaf iterator - Multiple calls to next() after exhaustion return nullptr") {
    auto cactus = std::make_shared<Cactus>();
    auto iter = cactus->createIterator();
    
    iter->next();  // Consume the plant
    CHECK_FALSE(iter->hasNext());
    
    for (int i = 0; i < 5; ++i) {
        CHECK(iter->next().get() == nullptr);
        CHECK_FALSE(iter->hasNext());
    }
}

TEST_CASE("Empty group iterator - Returns only the group itself") {
    auto emptyGroup = std::make_shared<Group>("EmptyPlot", true);
    auto iter = emptyGroup->createIterator();
    
    REQUIRE(iter != nullptr);
    CHECK(iter->hasNext());
    
    auto first = iter->next();
    CHECK(first.get() == emptyGroup.get());
    CHECK(first->getName() == "EmptyPlot");
    
    CHECK_FALSE(iter->hasNext());
}

TEST_CASE("Flat group - PreOrder traversal visits group then all children") {
    auto plot = std::make_shared<Group>("Plot1", true);
    auto rose = std::make_shared<Rose>();
    auto cactus = std::make_shared<Cactus>();
    auto basil = std::make_shared<Basil>();
    
    plot->add(rose);
    plot->add(cactus);
    plot->add(basil);
    
    auto iter = plot->createIterator();
    REQUIRE(iter != nullptr);
    
    // Expected order: Plot, Rose, Cactus, Basil
    std::vector<std::string> names;
    while (iter->hasNext()) {
        auto component = iter->next();
        names.push_back(component->getName());
    }
    
    REQUIRE(names.size() == 4);
    CHECK(names[0] == "Plot1");     // Group first
    CHECK(names[1] == "Rose");      // Then children in order
    CHECK(names[2] == "Cactus");
    CHECK(names[3] == "Basil");
}

TEST_CASE("Nested groups - PreOrder traversal (depth-first)") {
    /* Structure:
    //       Nursery
    //      /        \
    //   Plot1      Plot2
    //   /   \        |
    // Rose Cactus  Basil
    */
    
    auto nursery = std::make_shared<Group>("Nursery", true);
    auto plot1 = std::make_shared<Group>("Plot1", true);
    auto plot2 = std::make_shared<Group>("Plot2", true);
    
    auto rose = std::make_shared<Rose>();
    auto cactus = std::make_shared<Cactus>();
    auto basil = std::make_shared<Basil>();
    
    plot1->add(rose);
    plot1->add(cactus);
    plot2->add(basil);
    
    nursery->add(plot1);
    nursery->add(plot2);
    
    // Use PreOrderTraversal explicitly
    auto iter = std::make_unique<CompositeIterator>(
        nursery,
        std::make_unique<PreOrderTraversal>()
    );
    
    // Expected PreOrder: Nursery, Plot1, Rose, Cactus, Plot2, Basil
    std::vector<std::string> names;
    while (iter->hasNext()) {
        names.push_back(iter->next()->getName());
    }
    
    REQUIRE(names.size() == 6);
    CHECK(names[0] == "Nursery");   // Root first
    CHECK(names[1] == "Plot1");     // First child
    CHECK(names[2] == "Rose");      // Plot1's first child (deep)
    CHECK(names[3] == "Cactus");    // Plot1's second child
    CHECK(names[4] == "Plot2");     // Second child of root
    CHECK(names[5] == "Basil");     // Plot2's child
}

TEST_CASE("Nested groups - LevelOrder traversal (breadth-first)") {
    /* Same structure as above:
    //       Nursery
    //      /        \
    //   Plot1      Plot2
    //   /   \        |
    // Rose Cactus  Basil
    */
    
    auto nursery = std::make_shared<Group>("Nursery", true);
    auto plot1 = std::make_shared<Group>("Plot1", true);
    auto plot2 = std::make_shared<Group>("Plot2", true);
    
    auto rose = std::make_shared<Rose>();
    auto cactus = std::make_shared<Cactus>();
    auto basil = std::make_shared<Basil>();
    
    plot1->add(rose);
    plot1->add(cactus);
    plot2->add(basil);
    
    nursery->add(plot1);
    nursery->add(plot2);
    
    // Use LevelOrderTraversal explicitly
    auto iter = std::make_unique<CompositeIterator>(
        nursery,
        std::make_unique<LevelOrderTraversal>()
    );
    
    // Expected LevelOrder: Nursery, Plot1, Plot2, Rose, Cactus, Basil
    std::vector<std::string> names;
    while (iter->hasNext()) {
        names.push_back(iter->next()->getName());
    }
    
    REQUIRE(names.size() == 6);
    CHECK(names[0] == "Nursery");   // Level 0
    CHECK(names[1] == "Plot1");     // Level 1
    CHECK(names[2] == "Plot2");     // Level 1
    CHECK(names[3] == "Rose");      // Level 2
    CHECK(names[4] == "Cactus");    // Level 2
    CHECK(names[5] == "Basil");     // Level 2
}

TEST_CASE("Deep nesting - PreOrder handles 3+ levels correctly") {
    /* Structure:
    //           Nursery
    //          /       \
    //      Section1   Section2
    //        /            |
    //     Plot1         Plot2
    //      / \            |
    //   Rose Cactus     Basil
    */
    
    auto nursery = std::make_shared<Group>("Nursery", true);
    auto section1 = std::make_shared<Group>("Section1", true);
    auto section2 = std::make_shared<Group>("Section2", true);
    auto plot1 = std::make_shared<Group>("Plot1", true);
    auto plot2 = std::make_shared<Group>("Plot2", true);
    
    auto rose = std::make_shared<Rose>();
    auto cactus = std::make_shared<Cactus>();
    auto basil = std::make_shared<Basil>();
    
    plot1->add(rose);
    plot1->add(cactus);
    plot2->add(basil);
    section1->add(plot1);
    section2->add(plot2);
    nursery->add(section1);
    nursery->add(section2);
    
    auto iter = nursery->createIterator();
    
    // Expected: Nursery, Section1, Plot1, Rose, Cactus, Section2, Plot2, Basil
    std::vector<std::string> names;
    while (iter->hasNext()) {
        names.push_back(iter->next()->getName());
    }
    
    REQUIRE(names.size() == 8);
    CHECK(names[0] == "Nursery");
    CHECK(names[1] == "Section1");
    CHECK(names[2] == "Plot1");
    CHECK(names[3] == "Rose");
    CHECK(names[4] == "Cactus");
    CHECK(names[5] == "Section2");
    CHECK(names[6] == "Plot2");
    CHECK(names[7] == "Basil");
}

TEST_CASE("Iterator state - hasNext() is idempotent") {
    auto plot = std::make_shared<Group>("Plot", true);
    plot->add(std::make_shared<Rose>());
    
    auto iter = plot->createIterator();
    
    for (int i = 0; i < 10; ++i) {
        CHECK(iter->hasNext());  // Shouldn't change state
    }
    
    iter->next();  // Consume one
    for (int i = 0; i < 10; ++i) {
        CHECK(iter->hasNext());  // Still has the Rose
    }
}

TEST_CASE("Iterator state - Can iterate multiple times with separate iterators") {
    auto plot = std::make_shared<Group>("Plot", true);
    plot->add(std::make_shared<Rose>());
    plot->add(std::make_shared<Cactus>());
    
    auto iter1 = plot->createIterator();
    auto iter2 = plot->createIterator();
    
    // Exhaust first iterator
    int count1 = 0;
    while (iter1->hasNext()) {
        iter1->next();
        count1++;
    }
    
    // Second iterator should still work
    int count2 = 0;
    while (iter2->hasNext()) {
        iter2->next();
        count2++;
    }
    
    CHECK(count1 == 3);  // Plot + 2 plants
    CHECK(count2 == 3);  // Same
}

TEST_CASE("Practical use - Calculate total price using iterator") {
    auto plot = std::make_shared<Group>("Plot", true);
    plot->add(std::make_shared<Rose>());     // R135
    plot->add(std::make_shared<Cactus>());   // R120
    plot->add(std::make_shared<Basil>());    // R90
    
    auto iter = plot->createIterator();
    double totalPrice = 0.0;
    
    while (iter->hasNext()) {
        auto component = iter->next();
        totalPrice += component->getPrice();
    }
    
    // Plot itself has price 0, plants add up to R135 + R120 + R90 = R345
    CHECK(totalPrice == doctest::Approx(345.0));
}

TEST_CASE("Null safety - Traversal handles nullptr gracefully") {
    std::vector<std::shared_ptr<InventoryComponent>> collection;
    
    PreOrderTraversal preOrder;
    preOrder.traverse(nullptr, collection);
    CHECK(collection.empty());
    
    LevelOrderTraversal levelOrder;
    levelOrder.traverse(nullptr, collection);
    CHECK(collection.empty());
}
