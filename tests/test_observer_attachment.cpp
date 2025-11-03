// I ran this and it worked, but needed changes to nursery to expose command log
// which is dumb so im not commiting it
// #include "../include/Core/Nursery.h"
// #include "../include/Patterns/Factory/RoseFactory.h"
// #include "../include/Components/Rose.h"
// #include "../include/Components/Group.h"
// #include "../include/Core/Inventory.h"
// #include "../include/doctest.h"
// #include <fstream>
// #include <string>
// #include <cstdio>
// #include "../include/Utils/CommandLog.h"

// TEST_CASE("Observer attachment - supervisor attached to plants added to groups") {
//     // Create a nursery (this sets up the supervisor and registers callbacks)
//     auto nursery = Nursery::createAndSetup();
//     auto inventory = nursery->getInventory();

//     // Get the Rose factory
//     const auto& factories = nursery->getPlantFactories();
//     auto roseFactoryIt = factories.find("Rose");
//     CHECK(roseFactoryIt != factories.end());

//     // Create a plot and add to inventory
//     auto plot = std::make_shared<Group>("Test Plot", true);
//     inventory->add(plot);

//     // Create a plant using factory and add to plot
//     auto plant = roseFactoryIt->second->createPlant();
//     CHECK(plant);
//     plot->add(plant);

//     // Lower water level so supervisor will enqueue a WaterPlantCommand when notified
//     plant->setWaterLevel(10);

//     // Trigger notification which should notify the NurserySupervisor and enqueue a command
//     plant->notify();

//     // Inspect the nursery's command log to ensure a command was added for the current step
//     auto log = nursery->getCommandLog();
//     REQUIRE(log);
//     // Current step for a newly created nursery is 0; ensure there is at least one pending entry
//     auto pending = log->remainingPendingTextsForStep(0);
//     CHECK(!pending.empty());
// }