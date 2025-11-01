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
    std::shared_ptr<Group> ownerGroup;
    std::unique_ptr<PlantState> state;

   public:
    explicit DummyPlant(uint64_t id_) : Plant("Dummy", 0.0), id(id_) {}

    uint64_t getId() const { return id; }

    void water() {}

    std::shared_ptr<InventoryComponent> clone() const { return std::make_shared<DummyPlant>(id); }
    std::shared_ptr<InventoryComponent> blueprintClone() const { return clone(); }

    void setOwner(const std::shared_ptr<Group>& g) { ownerGroup = g; }
    std::shared_ptr<Group> getOwner() const { return ownerGroup; }

    void setState(std::unique_ptr<PlantState> s) { state = std::move(s); }
    PlantState* getState() const { return state.get(); }
};

// -----------------------------------------------------------------------------
// Dummy Group
// -----------------------------------------------------------------------------
class DummyGroup : public Group {
    std::vector<std::shared_ptr<InventoryComponent>> memberList;

   public:
    DummyGroup(const std::string& name) : Group(name, false) {}

    void add(const std::shared_ptr<InventoryComponent>& c) {
        if (!c) return;

        auto prevOwner = c->getOwner();
        if (prevOwner) prevOwner->remove(c);

        memberList.push_back(c);

        c->setOwner(static_pointer_cast<Group>(shared_from_this()));
    }

    void remove(const std::shared_ptr<InventoryComponent>& c) {
        memberList.erase(std::remove(memberList.begin(), memberList.end(), c), memberList.end());
        c->setOwner(nullptr);
    }

    const std::vector<std::shared_ptr<InventoryComponent>>& members() const { return memberList; }
};

// -----------------------------------------------------------------------------
// Dummy Inventory
// -----------------------------------------------------------------------------
class DummyInventory : public Inventory {
    std::shared_ptr<DummyGroup> storageGroup;

   public:
    DummyInventory() { storageGroup = std::make_shared<DummyGroup>("Storage"); }

    std::shared_ptr<DummyGroup> getStorageGroup() { return storageGroup; }

    std::shared_ptr<Group> findGroupByName(const std::string& name) const {
        if (name == "Storage") return storageGroup;
        return nullptr;
    }
};

// -----------------------------------------------------------------------------
// Tests
// -----------------------------------------------------------------------------
TEST_CASE("AddToStorageCommand - Constructor initializes correctly") {
    auto plant = std::make_shared<DummyPlant>(1);
    plant->setState(std::make_unique<Mature>());
    auto inventory = std::make_shared<DummyInventory>();

    AddToStorageCommand cmd(plant, inventory);

    CHECK(cmd.getTargetId() == plant->getId());
    CHECK(cmd.getStatus() == AddToStorageCommand::Status::Pending);
}

TEST_CASE("AddToStorageCommand - Execute handles null parameters safely") {
    auto plant = std::make_shared<DummyPlant>(42);
    plant->setState(std::make_unique<Mature>());
    auto inventory = std::make_shared<DummyInventory>();

    SUBCASE("Null plant") {
        AddToStorageCommand cmd(nullptr, inventory);
        cmd.execute();
        CHECK(cmd.getStatus() == AddToStorageCommand::Status::Failed);
    }

    SUBCASE("Null inventory") {
        AddToStorageCommand cmd(plant, nullptr);
        cmd.execute();
        CHECK(cmd.getStatus() == AddToStorageCommand::Status::Failed);
    }
}

TEST_CASE("AddToStorageCommand - Execute adds plant to storage group") {
    auto plant = std::make_shared<DummyPlant>(10);
    plant->setState(std::make_unique<Mature>());
    auto inventory = std::make_shared<DummyInventory>();
    auto storage = inventory->getStorageGroup();

    auto owner = std::make_shared<DummyGroup>("Owner");
    owner->add(plant);

    AddToStorageCommand cmd(plant, inventory);
    cmd.execute();

    CHECK(cmd.getStatus() == AddToStorageCommand::Status::Completed);
    CHECK(std::any_of(storage->members().begin(), storage->members().end(),
                      [&](const std::shared_ptr<InventoryComponent>& c) { return c == plant; }));
    CHECK(owner->members().empty());
}

TEST_CASE("AddToStorageCommand - Status and TargetId mutators work correctly") {
    auto plant = std::make_shared<DummyPlant>(33);
    plant->setState(std::make_unique<Mature>());
    auto inventory = std::make_shared<DummyInventory>();

    AddToStorageCommand cmd(plant, inventory);

    cmd.setStatus(AddToStorageCommand::Status::Failed);
    CHECK(cmd.getStatus() == AddToStorageCommand::Status::Failed);

    cmd.setStatus(AddToStorageCommand::Status::Completed);
    CHECK(cmd.getStatus() == AddToStorageCommand::Status::Completed);

    cmd.setTargetId(98765);
    CHECK(cmd.getTargetId() == 98765);
}
