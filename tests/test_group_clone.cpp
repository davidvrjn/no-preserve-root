/**
 * @file test_group_clone.cpp
 * @brief Tests for Group clone and blueprintClone methods
 *
 * Verifies that Groups can be properly cloned with all their children,
 * preserving structure and ownership relationships.
 */

#include "Components/Cactus.h"
#include "Components/Group.h"
#include "Components/Plant.h"
#include "Components/Rose.h"
#include "doctest.h"

TEST_SUITE("Group Cloning") {
    TEST_CASE("Group clone() - Simple group with plants") {
        auto original = std::make_shared<Group>("Plot 1", true);
        auto rose = std::make_shared<Rose>();
        auto cactus = std::make_shared<Cactus>();

        original->add(rose);
        original->add(cactus);

        // Clone the group
        auto cloned = std::dynamic_pointer_cast<Group>(original->clone());

        REQUIRE(cloned.get() != nullptr);
        CHECK(cloned->getName() == "Plot 1");
        CHECK(cloned->owns() == true);
        CHECK(cloned->getId() == original->getId());  // ID preserved for state restoration

        // Verify children were cloned
        auto clonedMembers = cloned->members();
        CHECK(clonedMembers.size() == 2);

        // Cloned children should be different objects
        auto originalMembers = original->members();
        CHECK(clonedMembers[0].get() != originalMembers[0].get());
        CHECK(clonedMembers[1].get() != originalMembers[1].get());

        // But should have same properties
        CHECK(clonedMembers[0]->getName() == originalMembers[0]->getName());
        CHECK(clonedMembers[1]->getName() == originalMembers[1]->getName());
    }

    TEST_CASE("Group blueprintClone() - Creates fresh copy") {
        auto original = std::make_shared<Group>("Storage", true);
        auto rose = std::make_shared<Rose>();

        original->add(rose);

        // Blueprint clone the group
        auto cloned = std::dynamic_pointer_cast<Group>(original->blueprintClone());

        REQUIRE(cloned.get() != nullptr);
        CHECK(cloned->getName() == "Storage");
        CHECK(cloned->owns() == true);
        CHECK(cloned->getId() != original->getId());  // New ID (fresh copy)

        // Verify children were blueprint cloned
        auto clonedMembers = cloned->members();
        CHECK(clonedMembers.size() == 1);

        // Should be different objects with different IDs
        auto originalMembers = original->members();
        CHECK(clonedMembers[0].get() != originalMembers[0].get());
        CHECK(clonedMembers[0]->getId() != originalMembers[0]->getId());
    }

    TEST_CASE("Group clone() - Nested groups (deep copy)") {
        auto topLevel = std::make_shared<Group>("Nursery", true);
        auto section1 = std::make_shared<Group>("Section 1", true);
        auto section2 = std::make_shared<Group>("Section 2", true);

        auto rose = std::make_shared<Rose>();
        auto cactus = std::make_shared<Cactus>();

        section1->add(rose);
        section2->add(cactus);
        topLevel->add(section1);
        topLevel->add(section2);

        // Clone the entire hierarchy
        auto cloned = std::dynamic_pointer_cast<Group>(topLevel->clone());

        REQUIRE(cloned.get() != nullptr);
        CHECK(cloned->getName() == "Nursery");

        // Verify nested structure was preserved
        auto clonedChildren = cloned->members();
        CHECK(clonedChildren.size() == 2);

        auto clonedSection1 = std::dynamic_pointer_cast<Group>(clonedChildren[0]);
        auto clonedSection2 = std::dynamic_pointer_cast<Group>(clonedChildren[1]);

        REQUIRE(clonedSection1.get() != nullptr);
        REQUIRE(clonedSection2.get() != nullptr);

        CHECK(clonedSection1->getName() == "Section 1");
        CHECK(clonedSection2->getName() == "Section 2");

        // Verify plants in nested groups were cloned
        CHECK(clonedSection1->members().size() == 1);
        CHECK(clonedSection2->members().size() == 1);
    }

    TEST_CASE("Group clone() - Empty group") {
        auto original = std::make_shared<Group>("Empty Plot", true);

        auto cloned = std::dynamic_pointer_cast<Group>(original->clone());

        REQUIRE(cloned.get() != nullptr);
        CHECK(cloned->getName() == "Empty Plot");
        CHECK(cloned->members().empty());
    }

    TEST_CASE("Group clone() - Non-owning group (references not cloned)") {
        auto refGroup = std::make_shared<Group>("View Group", false);
        auto rose = std::make_shared<Rose>();

        refGroup->add(rose);  // Adds as reference, not owned

        auto cloned = std::dynamic_pointer_cast<Group>(refGroup->clone());

        REQUIRE(cloned.get() != nullptr);
        CHECK(cloned->getName() == "View Group");
        CHECK(cloned->owns() == false);

        // Referenced children are NOT cloned (they're just weak references)
        // The clone would have empty references since we don't preserve weak_ptrs
        CHECK(cloned->members().empty());
    }

    TEST_CASE("Group clone() - Price calculation works on cloned groups") {
        auto original = std::make_shared<Group>("Garden", true);
        auto rose = std::make_shared<Rose>();      // Price: 135
        auto cactus = std::make_shared<Cactus>();  // Price varies

        original->add(rose);
        original->add(cactus);

        double originalPrice = original->getPrice();

        auto cloned = std::dynamic_pointer_cast<Group>(original->clone());
        double clonedPrice = cloned->getPrice();

        CHECK(clonedPrice == originalPrice);
        CHECK(clonedPrice > 0);
    }

    TEST_CASE("Group blueprintClone() - Nested groups get fresh IDs") {
        auto original = std::make_shared<Group>("Template", true);
        auto subgroup = std::make_shared<Group>("Sub", true);
        auto rose = std::make_shared<Rose>();

        subgroup->add(rose);
        original->add(subgroup);

        uint64_t originalId = original->getId();
        uint64_t subgroupId = subgroup->getId();
        uint64_t roseId = rose->getId();

        auto cloned = std::dynamic_pointer_cast<Group>(original->blueprintClone());

        // All IDs should be different (fresh blueprint)
        CHECK(cloned->getId() != originalId);

        auto clonedSub = std::dynamic_pointer_cast<Group>(cloned->members()[0]);
        CHECK(clonedSub->getId() != subgroupId);

        auto clonedRose = clonedSub->members()[0];
        CHECK(clonedRose->getId() != roseId);
    }

    TEST_CASE("Group clone() - Ownership chain preserved") {
        auto parent = std::make_shared<Group>("Parent", true);
        auto rose = std::make_shared<Rose>();

        parent->add(rose);

        // Verify original ownership
        CHECK(rose->getOwner().get() == parent.get());

        auto clonedParent = std::dynamic_pointer_cast<Group>(parent->clone());
        auto clonedRose = clonedParent->members()[0];

        // Cloned plant should have cloned parent as owner
        CHECK(clonedRose->getOwner().get() == clonedParent.get());
        CHECK(clonedRose->getOwner().get() != parent.get());
    }
}
