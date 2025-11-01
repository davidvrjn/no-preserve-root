#include "../include/Patterns/Command/AddToStorageCommand.h"

#include "../include/Components/Group.h"
#include "../include/Core/Inventory.h"
#include "../include/Components/Plant.h"

#include "../include/doctest.h"

#include <algorithm>
#include <memory>
#include <vector>
#include <string>

using namespace std;

// -----------------------------------------------------------------------------
// Dummy implementations
// -----------------------------------------------------------------------------

// Simple mock Plant that can be added to Groups
class DummyPlant : public Plant {
    uint64_t id;

public:
    explicit DummyPlant(uint64_t id) : id(id) {}
    uint64_t getId() const override { return id; }
};

// Dummy Inventory that provides a storage group
class DummyInventory : public Inventory {
    shared_ptr<Group> storageGroup;

public:
    DummyInventory() {
        // Create a non-owning storage group (doesn't take ownership of plants)
        storageGroup = make_shared<Group>("Storage", false);
    }

    shared_ptr<Group> getStorageGroup() override { return storageGroup; }
};

// -----------------------------------------------------------------------------
// Tests
// -----------------------------------------------------------------------------

TEST_CASE("AddToStorageCommand - Constructor initializes correctly") {
    auto plant = make_shared<DummyPlant>(1);
    auto inventory = make_shared<DummyInventory>();

    AddToStorageCommand cmd(plant, inventory);

    CHECK(cmd.getTargetId() == inventory->getStorageGroup()->getId());
    CHECK(cmd.getStatus() == Command::Status::Pending);
}

TEST_CASE("AddToStorageCommand - Execute handles null parameters safely") {
    auto plant = make_shared<DummyPlant>(42);
    auto inventory = make_shared<DummyInventory>();

    SUBCASE("Null plant") {
        AddToStorageCommand cmd(nullptr, inventory);
        cmd.execute();
        CHECK(cmd.getStatus() == Command::Status::Failed);
    }

    SUBCASE("Null inventory") {
        AddToStorageCommand cmd(plant, nullptr);
        cmd.execute();
        CHECK(cmd.getStatus() == Command::Status::Failed);
    }
}

TEST_CASE("AddToStorageCommand - Execute adds plant to storage group") {
    auto plant = make_shared<DummyPlant>(10);
    auto inventory = make_shared<DummyInventory>();
    auto storage = inventory->getStorageGroup();

    // Ensure plant not in group before
    CHECK_FALSE(any_of(
        storage->members().begin(), storage->members().end(),
        [&](const shared_ptr<InventoryComponent>& c) { return c.get() == plant.get(); }));

    AddToStorageCommand cmd(plant, inventory);
    cmd.execute();

    CHECK(cmd.getStatus() == Command::Status::Completed);
    CHECK(any_of(
        storage->members().begin(), storage->members().end(),
        [&](const shared_ptr<InventoryComponent>& c) { return c.get() == plant.get(); }));
}

TEST_CASE("AddToStorageCommand - Status and TargetId mutators work correctly") {
    auto plant = make_shared<DummyPlant>(33);
    auto inventory = make_shared<DummyInventory>();

    AddToStorageCommand cmd(plant, inventory);

    cmd.setStatus(Command::Status::Failed);
    CHECK(cmd.getStatus() == Command::Status::Failed);

    cmd.setStatus(Command::Status::Completed);
    CHECK(cmd.getStatus() == Command::Status::Completed);

    cmd.setTargetId(98765);
    CHECK(cmd.getTargetId() == 98765);
}
