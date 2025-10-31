#include "../include/doctest.h"

#include <iostream>
#include <memory>
#include <cmath>

#include "../include/Core/SaveSystem.h"
#include "../include/Core/Nursery.h"
#include "../include/Patterns/Memento/Memento.h"

TEST_CASE("Manual spawn verification high-rep") {
    SaveSystem ss;
    std::string filename = "saves/high_rep_save.json";

    REQUIRE(ss.fileExists(filename));

    auto m = ss.load(filename);
    REQUIRE(m != nullptr);

    auto nursery = std::make_shared<Nursery>();
    nursery->restoreFromMemento(m.get());

    std::cout << "Loaded nursery: money=" << nursery->getMoney()
              << ", reputation=" << nursery->getReputation()
              << ", knownTypes=" << nursery->getKnownPlantTypes().size() << "\n";

    // Replicate spawnChance calculation to inspect probability
    int reputation = nursery->getReputation();
    double repFactor;
    if (reputation == 0) repFactor = 0;
    else if (reputation <= 25) repFactor = reputation / 2;
    else if (reputation <= 50) repFactor = reputation / 3 + 4;
    else if (reputation <= 75) repFactor = reputation / 2 + (reputation/3 -22);
    else repFactor = reputation / 1.86;
    repFactor = std::ceil(repFactor);
    int spawnChance = 30 + static_cast<int>(repFactor);
    std::cout << "Calculated spawnChance: " << spawnChance << "%\n";

    nursery->startNewDay();

    for (int i = 0; i < 5; ++i) {
        nursery->advanceStep();
        std::cout << "Step " << i << " completedCommands=" << nursery->getCompletedCommandsThisStep().size()
                  << " remaining=" << nursery->getRemainingCommandsAtStepEnd().size()
                  << " customersLeft=" << nursery->getCustomersLeftThisStep() << "\n";
    }
}
