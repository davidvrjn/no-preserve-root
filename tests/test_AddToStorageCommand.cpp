#include "../include/Patterns/Command/AddToStorageCommand.h"

#include <algorithm>
#include <memory>
#include <vector>

#include "../include/doctest.h"

// ---------------------------------------------------------
// Dummy Implementations that inherit from real base classes
// ---------------------------------------------------------

class DummyPlant : public Plant {
    uint64_t id;
public:
    explicit DummyPlant(uint64_t id) : id(id) {}
    uint64_t getId() const override { return id; }
};

class DummyGroup : public Group {
    uint64_t id;
    std::vector<std::shared_ptr<Plant>> plants;
public:
    explicit DummyGroup(uint64_t id) : id(id) {}
    uint64_t getId() const override { return id; }

    void add(const std::shared_ptr<Plant>& plant) override {
        if (plant) plants.push_back(plant);
    }

    void remove(const std::shared_ptr<Plant>& plant) override {
        plants.erase(std::remove(plants.begin(), plants.end(), plant), plants.end());
    }

    bool contains(const std::shared_ptr<Plant>& plant) const {
        return std::find(plants.begin(), plants.end(), plant) != plants.end();
    }
};

class DummyInventoryComponent : public InventoryComponent {
    std::shared_ptr<DummyGroup> storageGroup;
public:
    DummyInventoryComponent() {
        storageGroup = std::make_shared<DummyGroup>(42);
    }

    std::shared_ptr<Group> getStorageGroup() override { return storageGroup; }
};

// ---------------------------------------------------------
// Test Cases
// ---------------------------------------------------------

TEST_CASE("AddToStorageCommand - Constructor initializes correctly") {
    auto plant = std::make_shared<DummyPlant>(5);
    auto inventory = std::make_shared<DummyInventoryComponent>();

    AddToStorageCommand cmd(plant, inventory);

    CHECK(cmd.getTargetId() == inventory->getStorageGroup()->getId());
    CHECK(cmd.getStatus() == Command::Status::Pending);
}

TEST_CASE("AddToStorageCommand - Execute fails with null pointers") {
    auto plant = std::make_shared<DummyPlant>(2);
    auto inventory = std::make_shared<DummyInventoryComponent>();

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
    auto inventory = std::make_shared<DummyInventoryComponent>();

    auto storage = std::dynamic_pointer_cast<DummyGroup>(inventory->getStorageGroup());
    REQUIRE(storage != nullptr);
    CHECK_FALSE(storage->contains(plant));

    AddToStorageCommand cmd(plant, inventory);
    cmd.execute();

    CHECK(cmd.getStatus() == Command::Status::Completed);
    CHECK(storage->contains(plant));
}

TEST_CASE("AddToStorageCommand - Status setters and getters work correctly") {
    auto plant = std::make_shared<DummyPlant>(3);
    auto inventory = std::make_shared<DummyInventoryComponent>();

    AddToStorageCommand cmd(plant, inventory);

    cmd.setStatus(Command::Status::Failed);
    CHECK(cmd.getStatus() == Command::Status::Failed);

    cmd.setStatus(Command::Status::Completed);
    CHECK(cmd.getStatus() == Command::Status::Completed);
}

TEST_CASE("AddToStorageCommand - Target ID setters and getters work correctly") {
    auto plant = std::make_shared<DummyPlant>(3);
    auto inventory = std::make_shared<DummyInventoryComponent>();

    AddToStorageCommand cmd(plant, inventory);

    cmd.setTargetId(12345);
    CHECK(cmd.getTargetId() == 12345);
}
