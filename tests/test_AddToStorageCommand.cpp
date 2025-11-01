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

// Minimal concrete subclass of Plant for testing
class DummyPlant : public Plant {
    uint64_t id;

public:
    // Plant requires name + price
    explicit DummyPlant(uint64_t id)
        : Plant("Dummy", 0.0), id(id) {}

    uint64_t getId() const override { return id; }

    // Implement pure virtuals
    void water() override {}
    shared_ptr<InventoryComponent> clone() const override {
        return make_shared<DummyPlant>(*this);
    }
};

// Dummy Inventory with a single storage group
class DummyInventory : public Inventory {
    shared_ptr<Group> storageGroup;

public:
    DummyInventory() {
        // Create a non-owning storage group
        storageGroup = make_shared<Group>("Storage", false);
    }

    // Not all Inventories define this as virtual, so skip 'override'
    shared_ptr<Group> getStorageGroup() { return storageGroup; }
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
