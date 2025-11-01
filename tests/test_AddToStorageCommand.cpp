#include <algorithm>
#include <memory>
#include <string>
#include <vector>

#include "../include/Components/Group.h"
#include "../include/Components/Plant.h"
#include "../include/Core/Inventory.h"
#include "../include/Patterns/Command/AddToStorageCommand.h"
#include "../include/doctest.h"

using namespace std;

// -----------------------------------------------------------------------------
// Dummy implementations
// -----------------------------------------------------------------------------

// Minimal concrete subclass of Plant for testing
class DummyPlant : public Plant {
    uint64_t id;

   public:
    explicit DummyPlant(uint64_t id) : Plant("Dummy", 0.0), id(id) {}

    uint64_t getId() const { return id; }  // remove override

    void water() override {}

    shared_ptr<InventoryComponent> clone() const override { return make_shared<DummyPlant>(id); }
};

// Minimal Group that stores members
class DummyGroup : public Group {
    vector<shared_ptr<InventoryComponent>> memberList;

   public:
    DummyGroup(const string& name) : Group(name, false) {}

    void add(const shared_ptr<InventoryComponent>& c) override { memberList.push_back(c); }

    const vector<shared_ptr<InventoryComponent>>& members() const { return memberList; }
};

// Dummy Inventory with a single storage group
class DummyInventory : public Inventory {
    shared_ptr<DummyGroup> storageGroup;

   public:
    DummyInventory() { storageGroup = make_shared<DummyGroup>("Storage"); }

    shared_ptr<DummyGroup> getStorageGroup() { return storageGroup; }
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
    CHECK_FALSE(
        any_of(storage->members().begin(), storage->members().end(),
               [&](const shared_ptr<InventoryComponent>& c) { return c.get() == plant.get(); }));

    AddToStorageCommand cmd(plant, inventory);
    cmd.execute();

    CHECK(cmd.getStatus() == Command::Status::Completed);
    CHECK(any_of(storage->members().begin(), storage->members().end(),
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
