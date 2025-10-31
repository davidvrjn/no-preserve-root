#include "../include/doctest.h"

#include <iostream>
#include <memory>

#include "../include/Core/SaveSystem.h"
#include "../include/Core/Nursery.h"
#include "../include/Patterns/Memento/Memento.h"

TEST_CASE("Manual spawn verification - load save and step") {
    SaveSystem ss;
    std::string filename = "saves/persistent_save_for_inspection.json";

    REQUIRE(ss.fileExists(filename));

    auto m = ss.load(filename);
    REQUIRE(m != nullptr);

    auto nursery = std::make_shared<Nursery>();
    nursery->restoreFromMemento(m.get());

    std::cout << "Loaded nursery: money=" << nursery->getMoney()
              << ", reputation=" << nursery->getReputation()
              << ", knownTypes=" << nursery->getKnownPlantTypes().size() << "\n";

    // Start new day to trigger daily plant updates
    nursery->startNewDay();
    std::cout << "After startNewDay - day=" << nursery->getCurrentDay()
              << ", phase=" << static_cast<int>(nursery->getCurrentPhase()) << "\n";

    for (int i = 0; i < 5; ++i) {
        bool ran = nursery->advanceStep();
        std::cout << "Step " << i << " ran=" << ran << "\n";
        auto completed = nursery->getCompletedCommandsThisStep();
        auto remaining = nursery->getRemainingCommandsAtStepEnd();
        std::cout << "  completed=" << completed.size() << "\n";
        for (const auto& s : completed) std::cout << "    + " << s << "\n";
        std::cout << "  remaining=" << remaining.size() << "\n";
        for (const auto& s : remaining) std::cout << "    - " << s << "\n";
        std::cout << "  customersLeft=" << nursery->getCustomersLeftThisStep() << "\n";
    }
}
