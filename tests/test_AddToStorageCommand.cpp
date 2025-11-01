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
// Dummy Plant
// -----------------------------------------------------------------------------
class DummyPlant : public Plant {
    uint64_t id;
    shared_ptr<Group> ownerGroup;
    unique_ptr<PlantState> state;

public:
    explicit DummyPlant(uint64_t id) : Plant("Dummy", 0.0), id(id) {}

    uint64_t getId() const { return id; }

    void water() {}

    shared_ptr<InventoryComponent> clone() const { return make_shared<DummyPlant>(id); }

    void setOwner(const shared_ptr<Group>& g) { ownerGroup = g; }
    shared_ptr<Group> getOwner() const { return ownerGroup; }

    void setState(unique_ptr<PlantState> s) { state = std::move(s); }
    PlantState* getState() const { return state.get(); }
};

// -----------------------------------------------------------------------------
// Dummy Group
// -----------------------------------------------------------------------------
class DummyGroup : public Group, public enable_shared_from_this<DummyGroup> {
    vector<shared_ptr<InventoryComponent>> memberList;

public:
    DummyGroup(const string& name) : Group(name, false) {}

    void add(const shared_ptr<InventoryComponent>& c) {
        memberList.push_back(c);
        c->setOwner(shared_from_this());  // update owner
    }

    void remove(const shared_ptr<InventoryComponent>& c) {
        memberList.erase(std::remove(memberList.begin(), memberList.end(), c), memberList.end());
    }

    const vector<shared_ptr<InventoryComponent>>& members() const { return memberList; }
};

// -----------------------------------------------------------------------------
// Dummy Inventory
// -----------------------------------------------------------------------------
class DummyInventory : public Inventory {
    shared_ptr<DummyGroup> storageGroup;

public:
    DummyInventory() { storageGroup = make_shared<DummyGroup>("Storage"); }

    shared_ptr<DummyGroup> getStorageGroup() { return storageGroup; }

    shared_ptr<Group> findGroupByName(const string& name) const {
        if (name == "Storage") return storageGroup;
        return nullptr;
    }
};

// -----------------------------------------------------------------------------
// Tests
// -----------------------------------------------------------------------------
TEST_CASE("AddToStorageCommand - Constructor initializes correctly") {
    auto plant = make_shared<DummyPlant>(1);
    plant->setState(make_unique<Mature>());
    auto inventory = make_shared<DummyInventory>();

    AddToStorageCommand cmd(plant, inventory);

    CHECK(cmd.getTargetId() == plant->getId());
    CHECK(cmd.getStatus() == Command::Status::Pending);
}

TEST_CASE("AddToStorageCommand - Execute handles null parameters safely") {
    auto plant = make_shared<DummyPlant>(42);
    plant->setState(make_unique<Mature>());
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
    plant->setState(make_unique<Mature>());
    auto inventory = make_shared<DummyInventory>();
    auto storage = inventory->getStorageGroup();

    auto owner = make_shared<DummyGroup>("Owner");
    plant->setOwner(owner);

    AddToStorageCommand cmd(plant, inventory);
    cmd.execute();

    CHECK(cmd.getStatus() == AddToStorageCommand::Status::Completed);
    CHECK(any_of(storage->members().begin(), storage->members().end(),
                 [&](const shared_ptr<InventoryComponent>& c) { return c.get() == plant.get(); }));
}

TEST_CASE("AddToStorageCommand - Status and TargetId mutators work correctly") {
    auto plant = make_shared<DummyPlant>(33);
    plant->setState(make_unique<Mature>());
    auto inventory = make_shared<DummyInventory>();

    AddToStorageCommand cmd(plant, inventory);

    cmd.setStatus(Command::Status::Failed);
    CHECK(cmd.getStatus() == Command::Status::Failed);

    cmd.setStatus(Command::Status::Completed);
    CHECK(cmd.getStatus() == Command::Status::Completed);

    cmd.setTargetId(98765);
    CHECK(cmd.getTargetId() == 98765);
}
