/**
 * Comprehensive Serialization Tests
 * Tests all aspects of save/load functionality including edge cases
 */

#include "../include/doctest.h"

#include <algorithm>
#include <fstream>
#include <memory>

#include "../include/Components/Cactus.h"
#include "../include/Components/Group.h"
#include "../include/Components/Plant.h"
#include "../include/Components/Rose.h"
#include "../include/Core/Inventory.h"
#include "../include/Core/Nursery.h"
#include "../include/Core/SaveSystem.h"
#include "../include/Patterns/Iterator/FilteredTraversal.h"
#include "../include/Patterns/Iterator/Iterator.h"
#include "../include/Patterns/Iterator/PreOrderTraversal.h"
#include "../include/Patterns/Memento/Memento.h"
#include "../include/Patterns/State/Growing.h"
#include "../include/Patterns/State/Mature.h"

TEST_CASE("SaveSystem - Basic save and load") {
    SaveSystem saveSystem;
    std::string filename = "test_save_basic.json";

    SUBCASE("Save creates a file") {
        auto nursery = std::make_shared<Nursery>();
        saveSystem.save(nursery, filename);

        CHECK(SaveSystem::fileExists(filename));
        std::remove(filename.c_str());
    }

    SUBCASE("Load returns valid memento") {
        auto nursery = std::make_shared<Nursery>();
        saveSystem.save(nursery, filename);

        auto memento = saveSystem.load(filename);
        CHECK(memento != nullptr);

        std::remove(filename.c_str());
    }

    SUBCASE("Load returns nullptr for nonexistent file") {
        auto memento = saveSystem.load("nonexistent_file.json");
        CHECK(memento == nullptr);
    }
}

TEST_CASE("SaveSystem - State preservation") {
    SaveSystem saveSystem;
    std::string filename = "test_save_state.json";

    SUBCASE("Preserves nursery metrics") {
        auto nursery = std::make_shared<Nursery>();
        nursery->adjustMoney(500);
        nursery->adjustReputation(25);

        saveSystem.save(nursery, filename);

        auto newNursery = std::make_shared<Nursery>();
        auto memento = saveSystem.load(filename);
        newNursery->restoreFromMemento(memento.get());

        CHECK(newNursery->getMoney() == 1500);
        CHECK(newNursery->getReputation() == 75);
        CHECK(newNursery->getCurrentDay() == 0);

        std::remove(filename.c_str());
    }

    SUBCASE("Preserves known plant types") {
        auto nursery = std::make_shared<Nursery>();
        nursery->addKnownPlantType("Rose");
        nursery->addKnownPlantType("Cactus");
        nursery->addKnownPlantType("Daisy");

        saveSystem.save(nursery, filename);

        auto newNursery = std::make_shared<Nursery>();
        auto memento = saveSystem.load(filename);
        newNursery->restoreFromMemento(memento.get());

        auto knownTypes = newNursery->getKnownPlantTypes();
        CHECK(knownTypes.size() == 3);
        CHECK(std::find(knownTypes.begin(), knownTypes.end(), "Rose") != knownTypes.end());
        CHECK(std::find(knownTypes.begin(), knownTypes.end(), "Cactus") != knownTypes.end());
        CHECK(std::find(knownTypes.begin(), knownTypes.end(), "Daisy") != knownTypes.end());

        std::remove(filename.c_str());
    }

    SUBCASE("Preserves inventory with plants") {
        auto nursery = std::make_shared<Nursery>();
        auto inventory = nursery->getInventory();

        auto rose = std::make_shared<Rose>();
        rose->setAge(10);
        rose->setHealth(95);
        inventory->add(rose);

        auto cactus = std::make_shared<Cactus>();
        cactus->setAge(5);
        cactus->setHealth(100);
        inventory->add(cactus);

        int originalCount = inventory->countAllComponents();
        int originalRoses = inventory->countByType("Rose");

        saveSystem.save(nursery, filename);

        auto newNursery = std::make_shared<Nursery>();
        auto memento = saveSystem.load(filename);
        newNursery->restoreFromMemento(memento.get());

        auto newInventory = newNursery->getInventory();
        CHECK(newInventory->countAllComponents() == originalCount);
        CHECK(newInventory->countByType("Rose") == originalRoses);
        CHECK(newInventory->countByType("Cactus") == 1);

        std::remove(filename.c_str());
    }

    SUBCASE("Preserves plant properties") {
        auto nursery = std::make_shared<Nursery>();
        auto inventory = nursery->getInventory();

        auto rose = std::make_shared<Rose>();
        rose->setAge(15);
        rose->setHealth(85);
        rose->setWaterLevel(60);
        rose->setState(std::make_unique<Mature>());
        inventory->add(rose);

        saveSystem.save(nursery, filename);

        auto newNursery = std::make_shared<Nursery>();
        auto memento = saveSystem.load(filename);
        newNursery->restoreFromMemento(memento.get());

        auto plants = newNursery->getInventory()->getAllPlants();
        bool foundRose = false;
        for (const auto& plant : plants) {
            if (plant->getName() == "Rose") {
                CHECK(plant->getAge() == 15);
                CHECK(plant->getHealth() == 85);
                CHECK(plant->getWaterLevel() == 60);
                foundRose = true;
                break;
            }
        }
        CHECK(foundRose);

        std::remove(filename.c_str());
    }

    SUBCASE("Preserves owning groups") {
        auto nursery = std::make_shared<Nursery>();
        auto inventory = nursery->getInventory();

        auto rose1 = std::make_shared<Rose>();
        auto rose2 = std::make_shared<Rose>();

        auto group = std::make_shared<Group>("Rose Garden");
        group->add(rose1);
        group->add(rose2);
        inventory->add(group);

        saveSystem.save(nursery, filename);

        auto newNursery = std::make_shared<Nursery>();
        auto memento = saveSystem.load(filename);
        newNursery->restoreFromMemento(memento.get());

        auto groups = newNursery->getInventory()->getAllGroups();
        bool foundGroup = false;
        for (const auto& g : groups) {
            if (g->getName() == "Rose Garden") {
                CHECK(g->members().size() == 2);
                foundGroup = true;
                break;
            }
        }
        CHECK(foundGroup);

        std::remove(filename.c_str());
    }
}

TEST_CASE("SaveSystem - File utilities") {
    SaveSystem saveSystem;
    std::string filename = "test_save_utils.json";

    SUBCASE("fileExists works correctly") {
        CHECK_FALSE(SaveSystem::fileExists(filename));

        auto nursery = std::make_shared<Nursery>();
        saveSystem.save(nursery, filename);

        CHECK(SaveSystem::fileExists(filename));

        std::remove(filename.c_str());
        CHECK_FALSE(SaveSystem::fileExists(filename));
    }

    SUBCASE("validateSaveFile detects valid JSON") {
        auto nursery = std::make_shared<Nursery>();
        saveSystem.save(nursery, filename);

        CHECK(SaveSystem::validateSaveFile(filename));

        std::remove(filename.c_str());
    }

    SUBCASE("validateSaveFile detects invalid JSON") {
        std::ofstream corrupt(filename);
        corrupt << "{ invalid json }\n}}}";
        corrupt.close();

        CHECK_FALSE(SaveSystem::validateSaveFile(filename));

        std::remove(filename.c_str());
    }

    SUBCASE("getSaveDay extracts correct day") {
        auto nursery = std::make_shared<Nursery>();
        saveSystem.save(nursery, filename);

        int day = SaveSystem::getSaveDay(filename);
        CHECK(day == 0);

        std::remove(filename.c_str());
    }

    SUBCASE("getSaveDay returns -1 for invalid file") {
        int day = SaveSystem::getSaveDay("nonexistent.json");
        CHECK(day == -1);
    }

    SUBCASE("prettyPrintSave returns formatted JSON") {
        auto nursery = std::make_shared<Nursery>();
        saveSystem.save(nursery, filename);

        std::string json = SaveSystem::prettyPrintSave(filename);
        CHECK_FALSE(json.empty());
        CHECK(json.find("currentDay") != std::string::npos);
        CHECK(json.find("money") != std::string::npos);

        std::remove(filename.c_str());
    }
}

TEST_CASE("SaveSystem - Complete round-trip") {
    SaveSystem saveSystem;
    std::string filename = "test_save_roundtrip.json";

    auto nursery = std::make_shared<Nursery>();

    // Setup complete state
    nursery->adjustMoney(250);
    nursery->adjustReputation(15);
    nursery->addKnownPlantType("Rose");
    nursery->addKnownPlantType("Cactus");

    auto inventory = nursery->getInventory();

    // Add individual plants
    auto rose1 = std::make_shared<Rose>();
    rose1->setAge(8);
    rose1->setState(std::make_unique<Growing>());
    inventory->add(rose1);

    auto cactus = std::make_shared<Cactus>();
    cactus->setAge(12);
    cactus->setState(std::make_unique<Mature>());
    inventory->add(cactus);

    // Add a group
    auto rose2 = std::make_shared<Rose>();
    rose2->setAge(5);
    auto rose3 = std::make_shared<Rose>();
    rose3->setAge(3);

    auto group = std::make_shared<Group>("Young Roses");
    group->add(rose2);
    group->add(rose3);
    inventory->add(group);

    // Save
    saveSystem.save(nursery, filename);

    // Load into new nursery
    auto newNursery = std::make_shared<Nursery>();
    auto memento = saveSystem.load(filename);
    REQUIRE(memento != nullptr);
    newNursery->restoreFromMemento(memento.get());

    // Verify all state
    CHECK(newNursery->getMoney() == 1250);
    CHECK(newNursery->getReputation() == 65);
    CHECK(newNursery->getCurrentDay() == 0);

    auto newInventory = newNursery->getInventory();
    CHECK(newInventory->countByType("Rose") == 3);
    CHECK(newInventory->countByType("Cactus") == 1);

    auto groups = newInventory->getAllGroups();
    bool foundYoungRoses = false;
    for (const auto& g : groups) {
        if (g->getName() == "Young Roses") {
            CHECK(g->members().size() == 2);
            foundYoungRoses = true;
            break;
        }
    }
    CHECK(foundYoungRoses);

    std::remove(filename.c_str());
}

TEST_CASE("SaveSystem - Deduplication with owning and non-owning groups") {
    SaveSystem saveSystem;
    std::string filename = "test_deduplication.json";
    
    SUBCASE("Non-owning groups don't duplicate components in serialization") {
        auto nursery = std::make_shared<Nursery>();
        auto inventory = nursery->getInventory();
        
        // Create standalone plants in inventory
        auto rose1 = std::make_shared<Rose>();
        rose1->setAge(5);
        inventory->add(rose1);
        
        auto rose2 = std::make_shared<Rose>();
        rose2->setAge(10);
        inventory->add(rose2);
        
        auto cactus = std::make_shared<Cactus>();
        cactus->setAge(8);
        inventory->add(cactus);
        
        // Create a non-owning reference group (ownsChildren=false)
        // This demonstrates that non-owning groups work for runtime references
        auto summerView = std::make_shared<Group>("Summer Plants View", false);
        summerView->add(rose1);  // Reference to rose1
        summerView->add(rose2);  // Reference to rose2
        inventory->add(summerView);
        
        // Verify initial state - deduplication prevents double-counting
        CHECK(inventory->countByType("Rose") == 2);
        CHECK(inventory->countByType("Cactus") == 1);
        CHECK(summerView->members().size() == 2);
        CHECK(rose1->getOwner() == nullptr);  // Not owned by view
        CHECK(rose2->getOwner() == nullptr);  // Not owned by view
        
        // Save
        saveSystem.save(nursery, filename);
        
        // Load
        auto newNursery = std::make_shared<Nursery>();
        auto memento = saveSystem.load(filename);
        REQUIRE(memento != nullptr);
        newNursery->restoreFromMemento(memento.get());
        
        auto newInventory = newNursery->getInventory();
        
        // Critical check: roses should not be duplicated
        CHECK(newInventory->countByType("Rose") == 2);
        CHECK(newInventory->countByType("Cactus") == 1);
        
        // Find the summer view group
        auto groups = newInventory->getAllGroups();
        std::shared_ptr<Group> restoredSummerView;
        for (const auto& g : groups) {
            if (g->getName() == "Summer Plants View") {
                restoredSummerView = g;
                break;
            }
        }
        
        REQUIRE(restoredSummerView != nullptr);
        CHECK(restoredSummerView->members().size() == 2);
        
        // Verify the view references are correct
        auto viewMembers = restoredSummerView->members();
        int roseCount = 0;
        for (const auto& member : viewMembers) {
            auto plant = std::dynamic_pointer_cast<Plant>(member);
            if (plant && plant->getName() == "Rose") {
                roseCount++;
                // Verify these roses are still standalone (not owned by view)
                CHECK(plant->getOwner() == nullptr);
            }
        }
        CHECK(roseCount == 2);
        
        std::remove(filename.c_str());
    }
}

TEST_CASE("SaveSystem - Filtered iterator views") {
    SaveSystem saveSystem;
    std::string filename = "test_filtered_views.json";
    
    SUBCASE("Use filtered iterators to populate non-owning view groups") {
        auto nursery = std::make_shared<Nursery>();
        auto inventory = nursery->getInventory();
        
        // Step 1: Create plots (owning groups) - plants are owned by plots
        auto outdoorPlot = std::make_shared<Group>("Outdoor Plot A", true);
        auto rose1 = std::make_shared<Rose>();
        rose1->setAge(5);
        auto rose2 = std::make_shared<Rose>();
        rose2->setAge(10);
        outdoorPlot->add(rose1);  // Plot owns the plant
        outdoorPlot->add(rose2);  // Plot owns the plant
        
        auto indoorPlot = std::make_shared<Group>("Indoor Plot B", true);
        auto cactus = std::make_shared<Cactus>();
        cactus->setAge(8);
        auto rose3 = std::make_shared<Rose>();
        rose3->setAge(3);
        indoorPlot->add(cactus);   // Plot owns the plant
        indoorPlot->add(rose3);    // Plot owns the plant
        
        inventory->add(outdoorPlot);
        inventory->add(indoorPlot);
        
        // Step 2: User wants to see "all summer plants" across all plots
        // Use filtered iterator to FIND the summer plants (roses in this case)
        // I have shown in the filtered iterator tests that it 
        // is possible to filter by season
        auto summerPlantsFilter = [](const std::shared_ptr<InventoryComponent>& comp) {
            return std::dynamic_pointer_cast<Rose>(comp) != nullptr;
        };
        
        auto filteredIter = inventory->createIterator();
        
        // Step 3: Create a VIEW group (non-owning) to hold references to summer plants
        auto summerPlantsView = std::make_shared<Group>("Summer Plants View", false);
        
        // Manually filter and add to view (in real app, this could be a helper method)
        while (filteredIter->hasNext()) {
            auto component = filteredIter->next();
            if (summerPlantsFilter(component)) {
                summerPlantsView->add(component);  // Adds weak_ptr reference, doesn't take ownership
            }
        }
        
        inventory->add(summerPlantsView);
        
        // Verify the view was populated correctly
        CHECK(summerPlantsView->members().size() == 3);  // All 3 roses
        CHECK(summerPlantsView->owns() == false);  // View doesn't own
        
        // Verify roses are still owned by their plots
        CHECK(rose1->getOwner().get() == outdoorPlot.get());
        CHECK(rose2->getOwner().get() == outdoorPlot.get());
        CHECK(rose3->getOwner().get() == indoorPlot.get());
        
        // Verify deduplication works - no double counting  
        CHECK(inventory->countByType("Rose") == 3);
        CHECK(inventory->countByType("Cactus") == 1);
        // Note: Temp root group from createIterator() is counted
        CHECK(inventory->countByType("Group") == 4);  // 2 plots + 1 view + 1 temp root
        // Total: 2 plots + 4 plants + 1 view + 1 temp root = 8
        int totalCount = inventory->countAllComponents();
        CHECK(totalCount == 8);
        
        // Save and load
        saveSystem.save(nursery, filename);
        
        auto newNursery = std::make_shared<Nursery>();
        auto memento = saveSystem.load(filename);
        REQUIRE(memento != nullptr);
        newNursery->restoreFromMemento(memento.get());
        
        auto newInventory = newNursery->getInventory();
        
        // After load, view group should be restored with correct references
        auto groups = newInventory->getAllGroups();
        std::shared_ptr<Group> restoredView;
        for (const auto& g : groups) {
            if (g->getName() == "Summer Plants View") {
                restoredView = g;
                break;
            }
        }
        
        REQUIRE(restoredView != nullptr);
        CHECK(restoredView->owns() == false);
        CHECK(restoredView->members().size() == 3);
        
        // Verify plants are still correctly owned by plots, not the view
        auto viewMembers = restoredView->members();
        for (const auto& member : viewMembers) {
            auto plant = std::dynamic_pointer_cast<Plant>(member);
            REQUIRE(plant != nullptr);
            // Each plant should be owned by a plot, not the view
            CHECK(plant->getOwner() != nullptr);
            CHECK(plant->getOwner().get() != restoredView.get());
        }
        
        // Deduplication still works after load
        CHECK(newInventory->countByType("Rose") == 3);
        CHECK(newInventory->countByType("Cactus") == 1);
        
        std::remove(filename.c_str());
    }
    
    SUBCASE("Filtered iterators help create and maintain view groups") {
        auto nursery = std::make_shared<Nursery>();
        auto inventory = nursery->getInventory();
        
        // Create multiple plots with various plants
        auto plot1 = std::make_shared<Group>("Plot 1", true);
        auto rose1 = std::make_shared<Rose>();
        rose1->setAge(3);
        auto cactus1 = std::make_shared<Cactus>();
        cactus1->setAge(5);
        plot1->add(rose1);
        plot1->add(cactus1);
        
        auto plot2 = std::make_shared<Group>("Plot 2", true);
        auto rose2 = std::make_shared<Rose>();
        rose2->setAge(7);
        auto rose3 = std::make_shared<Rose>();
        rose3->setAge(2);
        plot2->add(rose2);
        plot2->add(rose3);
        
        inventory->add(plot1);
        inventory->add(plot2);
        
        // Use filtered iterator to find all roses and create a view
        auto rosesView = std::make_shared<Group>("All Roses View", false);
        auto iter = inventory->createIterator();
        
        while (iter->hasNext()) {
            auto component = iter->next();
            if (std::dynamic_pointer_cast<Rose>(component)) {
                rosesView->add(component);  // Add reference to view
            }
        }
        
        inventory->add(rosesView);
        
        // View contains references to all 3 roses
        CHECK(rosesView->members().size() == 3);
        CHECK(rosesView->owns() == false);
        
        // Save, load, verify
        saveSystem.save(nursery, filename);
        
        auto newNursery = std::make_shared<Nursery>();
        auto memento = saveSystem.load(filename);
        newNursery->restoreFromMemento(memento.get());
        
        auto newInventory = newNursery->getInventory();
        
        // Find the restored view
        auto groups = newInventory->getAllGroups();
        std::shared_ptr<Group> restoredRosesView;
        for (const auto& g : groups) {
            if (g->getName() == "All Roses View") {
                restoredRosesView = g;
                break;
            }
        }
        
        REQUIRE(restoredRosesView != nullptr);
        CHECK(restoredRosesView->members().size() == 3);
        
        // All roses still counted correctly (no duplication)
        CHECK(newInventory->countByType("Rose") == 3);
        CHECK(newInventory->countByType("Cactus") == 1);
        
        std::remove(filename.c_str());
    }
}

