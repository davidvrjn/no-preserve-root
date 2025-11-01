#include <algorithm>
#include <memory>
#include <string>
#include <vector>

#include "../include/Components/Group.h"
#include "../include/Components/Plant.h"
#include "../include/Core/Inventory.h"
#include "../include/Patterns/Command/AddToStorageCommand.h"
#include "../include/Patterns/State/Mature.h"
#include "../include/doctest.h"

using namespace std;

// -----------------------------------------------------------------------------
// Dummy implementations
// -----------------------------------------------------------------------------

// Minimal dummy Mature state
class DummyMature : public Mature {
   public:
    DummyMature() : Mature() {}
};

// Minimal concrete Plant with state and owner
class DummyPlant : public Plant {
    uint64_t id;
    shared_ptr<Group> ownerGroup;

   public:
    explicit DummyPlant(uint64_t id) : Plant("Dummy", 0.0), id(id) {
        setState(make_unique<DummyMature>());
    }

    uint64_t getId() const { return id; }

    void water() override {}

    shared_ptr<InventoryComponent> clone() const override { return make_shared<DummyPlant>(id); }

    void setOwner(const shared_ptr<Group>& g) { ownerGroup = g; }
    shared_ptr<Group> getOwner() const { return ownerGroup; }
};

// Minimal Group that stores members
class DummyGroup : public Group {
    vector<shared_ptr<InventoryComponent>> memberList;

   public:
    DummyGroup(const string& name) : Group(name, false) {}

    void add(const std::shared_ptr<InventoryComponent>& c) override { memberList.push_back(c); }

    const vector<shared_ptr<InventoryComponent>>& members() const { return memberList; }
};

// Dummy Inventory with a single storage group
class DummyInventory : public Inventory {
    shared_ptr<DummyGroup> storageGroup;

   public:
    DummyInventory() { storageGroup = make_shared<DummyGroup>("Storage"); }

    shared_ptr<DummyGroup> getStorageGroup() { return storageGroup; }

    std::shared_ptr<Group> findGroupByName(const std::string& name) {
        if (name == "Storage") return storageGroup;
        return nullptr;
    }
};

// -----------------------------------------------------------------------------
// Tests
// -----------------------------------------------------------------------------

TEST_CASE("AddToStorageCommand - Constructor initializes correctly") {
    auto plant = make_shared<DummyPlant>(1);
    auto inventory = make_shared<DummyInventory>();

    AddToStorageCommand cmd(plant, inventory);

    // targetId is plant ID in AddToStorageCommand.cpp
    CHECK(cmd.getTargetId() == plant->getId());
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

    // Set a dummy owner so execute() works
    auto owner = make_shared<DummyGroup>("Owner");
    plant->setOwner(owner);

    // Ensure plant not in storage group before
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
