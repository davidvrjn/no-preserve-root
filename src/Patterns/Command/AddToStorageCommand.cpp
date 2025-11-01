#include <algorithm>
#include <memory>
#include <vector>

#include "../include/Patterns/Command/AddToStorageCommand.h"
#include "../include/doctest.h"

// ---------------- Dummy Classes ----------------

class Plant {
public:
    virtual ~Plant() = default;
    virtual uint64_t getId() const { return 0; }
};

class Inventory {
public:
    virtual ~Inventory() = default;
    virtual std::shared_ptr<class Group> getStorageGroup() { return nullptr; }
};

class Group {
public:
    virtual ~Group() = default;
    virtual void add(const std::shared_ptr<Plant>&) {}
    virtual void remove(const std::shared_ptr<Plant>&) {}
};

// Dummy plant /w ID
class DummyPlant : public Plant {
    uint64_t id;
public:
    explicit DummyPlant(uint64_t id) : id(id) {}
    uint64_t getId() const override { return id; }
};

// Dummy group that can hold plants
class DummyGroup : public Group {
    uint64_t id;
    std::vector<std::shared_ptr<Plant>> plants;
public:
    explicit DummyGroup(uint64_t id) : id(id) {}

    uint64_t getId() const { return id; }

    void add(const std::shared_ptr<Plant>& plant) override {
        plants.push_back(plant);
    }

    void remove(const std::shared_ptr<Plant>& plant) override {
        plants.erase(std::remove(plants.begin(), plants.end(), plant), plants.end());
    }

    bool contains(const std::shared_ptr<Plant>& plant) const {
        return std::find(plants.begin(), plants.end(), plant) != plants.end();
    }
};

// Dummy inventory with a storage group
class DummyInventory : public Inventory {
public:
    std::shared_ptr<DummyGroup> storageGroup;

    DummyInventory() {
        storageGroup = std::make_shared<DummyGroup>(42);
    }

    std::shared_ptr<DummyGroup> getStorageGroup() override { 
        return storageGroup; 
    }
};

// ---------------- Test Cases ----------------

TEST_CASE("AddToStorageCommand - Constructor initializes correctly") {
    auto plant = std::make_shared<DummyPlant>(5);
    auto inventory = std::make_shared<DummyInventory>();

    AddToStorageCommand cmd(plant, inventory);

    CHECK(cmd.getTargetId() == inventory->getStorageGroup()->getId());
    CHECK(cmd.getStatus() == Command::Status::Pending);
}

TEST_CASE("AddToStorageCommand - Execute fails with null pointers") {
    auto plant = std::make_shared<DummyPlant>(2);
    auto inventory = std::make_shared<DummyInventory>();

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

TEST_CASE("AddToStorageCommand - Successful execution moves plant to storage") {
    auto plant = std::make_shared<DummyPlant>(3);
    auto inventory = std::make_shared<DummyInventory>();

    auto storage = inventory->getStorageGroup();
    CHECK_FALSE(storage->contains(plant));

    AddToStorageCommand cmd(plant, inventory);
    cmd.execute();

    CHECK(cmd.getStatus() == Command::Status::Completed);
    CHECK(storage->contains(plant));
}

TEST_CASE("AddToStorageCommand - Status setters and getters work correctly") {
    auto plant = std::make_shared<DummyPlant>(3);
    auto inventory = std::make_shared<DummyInventory>();

    AddToStorageCommand cmd(plant, inventory);

    cmd.setStatus(Command::Status::Failed);
    CHECK(cmd.getStatus() == Command::Status::Failed);

    cmd.setStatus(Command::Status::Completed);
    CHECK(cmd.getStatus() == Command::Status::Completed);
}

TEST_CASE("AddToStorageCommand - Target ID setters and getters work correctly") {
    auto plant = std::make_shared<DummyPlant>(3);
    auto inventory = std::make_shared<DummyInventory>();

    AddToStorageCommand cmd(plant, inventory);

    cmd.setTargetId(12345);
    CHECK(cmd.getTargetId() == 12345);
}
