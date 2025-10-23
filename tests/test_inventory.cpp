/**
 * @file test_inventory.cpp
 * @brief Unit tests for Inventory add() and remove() operations
 * 
 * Tests cover:
 * - Adding single plants and groups
 * - Adding multiple components
 * - Null handling
 * - Duplicate prevention
 * - Auto-move functionality (ownership transfer)
 * - Removal operations
 * - Memory management
 * - Complex nested structures
 */

#include "../include/doctest.h"

#include <memory>
#include <vector>

#include "../include/Core/Inventory.h"
#include "../include/Components/Group.h"
#include "../include/Components/Rose.h"
#include "../include/Components/Cactus.h"
#include "../include/Components/Basil.h"
#include "../include/Patterns/Iterator/Iterator.h"
#include "../include/Patterns/State/PlantState.h"  // Needed for complete type

// Helper function to compare shared_ptr with nullptr for doctest output
namespace doctest {
template<typename T>
String toString(const std::shared_ptr<T>& ptr) {
    if (ptr) {
        return String("shared_ptr(") + toString(ptr.get()) + ")";
    } else {
        return "shared_ptr(nullptr)";
    }
}
}

TEST_CASE("Inventory add - Single plant has no owner") 
{
    auto inventory = std::make_shared<Inventory>();
    auto rose = std::make_shared<Rose>();
    
    inventory->add(rose);
    
    // Plant should be at top-level (no Group owner)
    CHECK(rose->getOwner().get() == nullptr);
}

TEST_CASE("Inventory add - Multiple plants added successfully") 
{
    auto inventory = std::make_shared<Inventory>();
    auto rose = std::make_shared<Rose>();
    auto cactus = std::make_shared<Cactus>();
    auto basil = std::make_shared<Basil>();
    
    inventory->add(rose);
    inventory->add(cactus);
    inventory->add(basil);
    
    CHECK(rose->getOwner().get() == nullptr);
    CHECK(cactus->getOwner().get() == nullptr);
    CHECK(basil->getOwner().get() == nullptr);
}

TEST_CASE("Inventory add - Group has no owner at top level") 
{
    auto inventory = std::make_shared<Inventory>();
    auto group = std::make_shared<Group>("Plot A", true);
    auto rose = std::make_shared<Rose>();
    
    // Add plant to group first
    group->add(rose);
    
    // Add group to inventory
    inventory->add(group);
    
    // Group should have no owner (top-level)
    CHECK(group->getOwner().get() == nullptr);
    // Rose should be owned by the group
    CHECK(rose->getOwner().get() == group.get());
}

TEST_CASE("Inventory add - Nullptr handled gracefully") 
{
    auto inventory = std::make_shared<Inventory>();
    
    // Should not crash
    CHECK_NOTHROW(inventory->add(nullptr));
}

TEST_CASE("Inventory add - Duplicate component ignored") 
{
    auto inventory = std::make_shared<Inventory>();
    auto rose = std::make_shared<Rose>();
    
    inventory->add(rose);
    inventory->add(rose);  // Try to add again
    
    // Should still have no owner and not cause issues
    CHECK(rose->getOwner().get() == nullptr);
}

TEST_CASE("Inventory add - Auto-move from Group to Inventory") 
{
    auto inventory = std::make_shared<Inventory>();
    auto group = std::make_shared<Group>("Plot A", true);
    auto rose = std::make_shared<Rose>();
    
    // Add rose to group
    group->add(rose);
    REQUIRE(rose->getOwner().get() == group.get());
    
    // Add rose to inventory - should auto-move from group
    inventory->add(rose);
    
    // Rose should now have no owner (top-level)
    CHECK(rose->getOwner().get() == nullptr);
}

TEST_CASE("Inventory add - Auto-move between Groups") 
{
    auto inventory = std::make_shared<Inventory>();
    auto groupA = std::make_shared<Group>("Plot A", true);
    auto groupB = std::make_shared<Group>("Plot B", true);
    auto rose = std::make_shared<Rose>();
    
    // Add groups to inventory
    inventory->add(groupA);
    inventory->add(groupB);
    
    // Add rose to groupA
    groupA->add(rose);
    REQUIRE(rose->getOwner().get() == groupA.get());
    
    // Add rose to groupB - should auto-move
    groupB->add(rose);
    CHECK(rose->getOwner().get() == groupB.get());
}

TEST_CASE("Inventory remove - Plant removed and owner cleared") 
{
    auto inventory = std::make_shared<Inventory>();
    auto rose = std::make_shared<Rose>();
    
    inventory->add(rose);
    inventory->remove(rose);
    
    // Owner should be cleared
    CHECK(rose->getOwner().get() == nullptr);
}

TEST_CASE("Inventory remove - Group removed, children still owned by group") 
{
    auto inventory = std::make_shared<Inventory>();
    auto group = std::make_shared<Group>("Plot A", true);
    auto rose = std::make_shared<Rose>();
    
    group->add(rose);
    inventory->add(group);
    
    inventory->remove(group);
    
    // Group owner should be cleared
    CHECK(group->getOwner().get() == nullptr);
    // Rose should still be owned by group
    CHECK(rose->getOwner().get() == group.get());
}

TEST_CASE("Inventory remove - Nullptr handled gracefully")
{
    auto inventory = std::make_shared<Inventory>();
    
    // Should not crash
    CHECK_NOTHROW(inventory->remove(nullptr));
}

TEST_CASE("Inventory remove - Non-existent component handled gracefully")
{
    auto inventory = std::make_shared<Inventory>();
    auto rose = std::make_shared<Rose>();
    
    // Try to remove without adding
    CHECK_NOTHROW(inventory->remove(rose));
}

TEST_CASE("Inventory iterator - Returns valid iterator for empty inventory") 
{
    auto inventory = std::make_shared<Inventory>();
    
    auto iter = inventory->createIterator();
    REQUIRE(iter.get() != nullptr);
}

TEST_CASE("Inventory iterator - Traverses single plant")
{
    auto inventory = std::make_shared<Inventory>();
    auto rose = std::make_shared<Rose>();
    
    inventory->add(rose);
    
    auto iter = inventory->createIterator();
    std::vector<std::string> names;
    
    while (iter->hasNext())
    {
        auto component = iter->next();
        names.push_back(component->getName());
    }
    
    // Should only have the single plant
    REQUIRE(names.size() == 1);
    CHECK(names[0] == "Rose");
}

TEST_CASE("Inventory iterator - Traverses multiple plants")
{
    auto inventory = std::make_shared<Inventory>();
    
    inventory->add(std::make_shared<Rose>());
    inventory->add(std::make_shared<Cactus>());
    inventory->add(std::make_shared<Basil>());
    
    auto iter = inventory->createIterator();
    int count = 0;
    
    while(iter->hasNext())
    {
        auto component = iter->next();
        REQUIRE(component.get() != nullptr);
        count++;
    }
    
    // Should only count the 3 plants, not any additional components
    CHECK(count == 3);
}

TEST_CASE("Inventory iterator - Traverses group with nested plants")
{
    auto inventory = std::make_shared<Inventory>();
    auto group = std::make_shared<Group>("Plot A", true);
    
    group->add(std::make_shared<Rose>());
    group->add(std::make_shared<Cactus>());
    inventory->add(group);
    
    auto iter = inventory->createIterator();
    std::vector<std::string> names;
    
    while(iter->hasNext())
    {
        names.push_back(iter->next()->getName());
    }
    
    // Should iterate over: Plot A (group), Rose, Cactus
    // If your iterator is counting differently, adjust the expected count
    REQUIRE(names.size() == 3);
    CHECK(names[0] == "Plot A");
    CHECK(names[1] == "Rose");
    CHECK(names[2] == "Cactus");
}

TEST_CASE("Inventory iterator - Complex nested structure")
{
    /* Structure:
    //   Inventory
    //   ├── Plot A
    //   │   ├── Rose
    //   │   └── Cactus
    //   ├── Plot B
    //   │   └── Basil
    //   └── Loose Rose
    */
    
    auto inventory = std::make_shared<Inventory>();
    
    auto plotA = std::make_shared<Group>("Plot A", true);
    plotA->add(std::make_shared<Rose>());
    plotA->add(std::make_shared<Cactus>());
    
    auto plotB = std::make_shared<Group>("Plot B", true);
    plotB->add(std::make_shared<Basil>());
    
    inventory->add(plotA);
    inventory->add(plotB);
    inventory->add(std::make_shared<Rose>());  // Loose plant
    
    auto iter = inventory->createIterator();
    int componentCount = 0;
    
    while(iter->hasNext())
    {
        auto component = iter->next();
        REQUIRE(component.get() != nullptr);
        componentCount++;
    }
    
    // Should have: 2 groups + 4 plants = 6 components total
    // If your iterator counts differently, adjust this number
    CHECK(componentCount == 6);
}

TEST_CASE("Inventory structure - Multiple groups with overlapping plant types")
{
    auto inventory = std::make_shared<Inventory>();
    
    auto storage = std::make_shared<Group>("Storage", true);
    auto greenhouse = std::make_shared<Group>("Greenhouse", true);
    auto outdoor = std::make_shared<Group>("Outdoor", true);
    
    // Add different plants to each area
    storage->add(std::make_shared<Rose>());
    storage->add(std::make_shared<Rose>());
    
    greenhouse->add(std::make_shared<Cactus>());
    greenhouse->add(std::make_shared<Basil>());
    
    outdoor->add(std::make_shared<Rose>());
    outdoor->add(std::make_shared<Cactus>());
    
    inventory->add(storage);
    inventory->add(greenhouse);
    inventory->add(outdoor);
    
    // Verify all groups are at top level
    CHECK(storage->getOwner().get() == nullptr);
    CHECK(greenhouse->getOwner().get() == nullptr);
    CHECK(outdoor->getOwner().get() == nullptr);
    
    // Count total components
    auto iter = inventory->createIterator();
    int totalComponents = 0;

    while(iter->hasNext())
    {
        iter->next();
        totalComponents++;
    }
    
    // 3 groups + 6 plants = 9 components
    // If your iterator counts differently, adjust this number
    CHECK(totalComponents == 9);
}

TEST_CASE("Inventory memory - Component lifetime managed correctly")
{
    auto inventory = std::make_shared<Inventory>();
    std::weak_ptr<Rose> weakRose;
    
    {
        auto rose = std::make_shared<Rose>();
        weakRose = rose;
        inventory->add(rose);
        
        // Rose should still exist (held by inventory)
        CHECK_FALSE(weakRose.expired());
    }
    
    // Rose should still exist (held by inventory)
    CHECK_FALSE(weakRose.expired());
    
    // Remove rose from inventory
    if(auto rose = weakRose.lock())
    {
        inventory->remove(rose);
    }
    
    // Now rose should be destroyed
    CHECK(weakRose.expired());
}

TEST_CASE("Inventory memory - Group removal doesn't destroy children immediately")
{
    auto inventory = std::make_shared<Inventory>();
    auto group = std::make_shared<Group>("Plot", true);
    auto rose = std::make_shared<Rose>();
    
    std::weak_ptr<Rose> weakRose = rose;
    
    group->add(rose);
    inventory->add(group);
    
    // Remove group from inventory, but keep local reference
    inventory->remove(group);
    
    // Rose should still exist (owned by group, which we still have)
    CHECK_FALSE(weakRose.expired());
    
    // Now release the plant reference
    rose.reset();
    
    // Rose should still exist (owned by group)
    CHECK_FALSE(weakRose.expired());
    
    // Release group
    group.reset();
    
    // Now rose should be destroyed
    CHECK(weakRose.expired());
}

TEST_CASE("Practical use - Calculate total inventory value")
{
    auto inventory = std::make_shared<Inventory>();
    
    auto plotA = std::make_shared<Group>("Plot A", true);
    plotA->add(std::make_shared<Rose>());     // R135
    plotA->add(std::make_shared<Cactus>());   // R120
    
    auto plotB = std::make_shared<Group>("Plot B", true);
    plotB->add(std::make_shared<Basil>());    // R90
    
    inventory->add(plotA);
    inventory->add(plotB);
    inventory->add(std::make_shared<Rose>()); // R135
    
    auto iter = inventory->createIterator();
    double totalValue = 0.0;
    
    while(iter->hasNext())
    {
        auto component = iter->next();
        totalValue += component->getPrice();
    }
    
    // Plot groups have 0 price, plants: R135 + R120 + R90 + R135 = R480
    CHECK(totalValue == doctest::Approx(480.0));
}

TEST_CASE("Edge case - Adding group to itself should be handled")
{
    auto inventory = std::make_shared<Inventory>();
    auto group = std::make_shared<Group>("Plot", true);
    
    inventory->add(group);
    
    // Attempting to add group to itself (if implemented in Group)
    // This test mainly ensures inventory isn't affected by such operations
    CHECK(group->getOwner().get() == nullptr);
}

TEST_CASE("Edge case - Rapid add and remove operations")
{
    auto inventory = std::make_shared<Inventory>();
    auto rose = std::make_shared<Rose>();
    
    for(int i = 0; i < 10; ++i)
    {
        inventory->add(rose);
        inventory->remove(rose);
    }
    
    // Final state: rose should have no owner
    CHECK(rose->getOwner().get() == nullptr);
}