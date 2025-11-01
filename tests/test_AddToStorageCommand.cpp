#include "../include/Patterns/Command/AddToStorageCommand.h"
#include "../include/Components/Group.h"
#include "../include/Components/Rose.h"
#include "../include/Core/Inventory.h"
#include "../include/Patterns/State/Mature.h"
#include "../include/Patterns/State/Seedling.h"
#include "../include/doctest.h"

TEST_CASE("AddToStorageCommand executes successfully with Mature plant") {
    auto inventory = std::make_shared<Inventory>();
    auto storage = std::make_shared<Group>("Storage");
    inventory->add(storage);

    auto plot = std::make_shared<Group>("Plot1");
    inventory->add(plot);

    auto plant = std::make_shared<Rose>();
    plot->add(plant);
    plant->setState(std::make_unique<Mature>());

    AddToStorageCommand cmd(plant, inventory);
    cmd.execute();

    CHECK_EQ(cmd.getStatus(), AddToStorageCommand::Status::Completed);

    // Plant should now be in Storage
    auto it = storage->createIterator();
    bool found = false;
    while (it->hasNext()) {
        if (it->next() == plant) {
            found = true;
            break;
        }
    }
    CHECK(found);

    // Plant should no longer be in original plot
    CHECK(plot->members().empty());
}

TEST_CASE("AddToStorageCommand fails if plant is nullptr") {
    auto inventory = std::make_shared<Inventory>();
    auto cmd = AddToStorageCommand(nullptr, inventory);

    cmd.execute();
    CHECK_EQ(cmd.getStatus(), AddToStorageCommand::Status::Failed);
}

TEST_CASE("AddToStorageCommand fails if inventory is nullptr") {
    auto plant = std::make_shared<Rose>();
    plant->setState(std::make_unique<Mature>());

    auto cmd = AddToStorageCommand(plant, nullptr);
    cmd.execute();

    CHECK_EQ(cmd.getStatus(), AddToStorageCommand::Status::Failed);
}

TEST_CASE("AddToStorageCommand fails if plant is not Mature") {
    auto inventory = std::make_shared<Inventory>();
    auto storage = std::make_shared<Group>("Storage");
    inventory->add(storage);

    auto plot = std::make_shared<Group>("Plot1");
    inventory->add(plot);

    auto plant = std::make_shared<Rose>();
    plot->add(plant);
    plant->setState(std::make_unique<Seedling>()); // Wrong state

    AddToStorageCommand cmd(plant, inventory);
    cmd.execute();

    CHECK_EQ(cmd.getStatus(), AddToStorageCommand::Status::Failed);

    // Plant should remain in original plot
    CHECK_FALSE(plot->members().empty());
}

TEST_CASE("AddToStorageCommand fails if plant has no owner") {
    auto inventory = std::make_shared<Inventory>();
    auto storage = std::make_shared<Group>("Storage");
    inventory->add(storage);

    auto plant = std::make_shared<Rose>();
    plant->setState(std::make_unique<Mature>());

    AddToStorageCommand cmd(plant, inventory);
    cmd.execute();

    CHECK_EQ(cmd.getStatus(), AddToStorageCommand::Status::Failed);
}

TEST_CASE("AddToStorageCommand fails if Storage group doesn't exist") {
    auto inventory = std::make_shared<Inventory>();

    auto plot = std::make_shared<Group>("Plot1");
    inventory->add(plot);

    auto plant = std::make_shared<Rose>();
    plot->add(plant);
    plant->setState(std::make_unique<Mature>());

    AddToStorageCommand cmd(plant, inventory);
    cmd.execute();

    CHECK_EQ(cmd.getStatus(), AddToStorageCommand::Status::Failed);
}

TEST_CASE("AddToStorageCommand stores correct target ID") {
    auto inventory = std::make_shared<Inventory>();
    auto storage = std::make_shared<Group>("Storage");
    inventory->add(storage);

    auto plot = std::make_shared<Group>("Plot1");
    inventory->add(plot);

    auto plant = std::make_shared<Rose>();
    plot->add(plant);
    plant->setState(std::make_unique<Mature>());

    AddToStorageCommand cmd(plant, inventory);

    CHECK_EQ(cmd.getTargetId(), plant->getId());
}

TEST_CASE("AddToStorageCommand can manually set and get status and target ID") {
    auto plant = std::make_shared<Rose>();
    auto inventory = std::make_shared<Inventory>();

    AddToStorageCommand cmd(plant, inventory);

    cmd.setStatus(AddToStorageCommand::Status::Cancelled);
    CHECK_EQ(cmd.getStatus(), AddToStorageCommand::Status::Cancelled);

    cmd.setTargetId(999);
    CHECK_EQ(cmd.getTargetId(), 999);
}
