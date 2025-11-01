#include "../include/Patterns/Command/AddToStorageCommand.h"

#include <algorithm>
#include <memory>
#include <vector>
#include "../include/doctest.h"

// ---------------------------------------------------------
// Lightweight dummy definitions (don’t inherit from incomplete types)
// ---------------------------------------------------------

// Mock Plant with minimal interface
class MockPlant {
    uint64_t id;
public:
    explicit MockPlant(uint64_t id) : id(id) {}
    uint64_t getId() const { return id; }
};

// Mock Group that can hold plants
class MockGroup {
    uint64_t id;
    std::vector<std::shared_ptr<MockPlant>> plants;
public:
    explicit MockGroup(uint64_t id) : id(id) {}

    uint64_t getId() const { return id; }

    void add(const std::shared_ptr<MockPlant>& plant) {
        plants.push_back(plant);
    }

    void remove(const std::shared_ptr<MockPlant>& plant) {
        plants.erase(std::remove(plants.begin(), plants.end(), plant), plants.end());
    }

    bool contains(const std::shared_ptr<MockPlant>& plant) const {
        return std::find(plants.begin(), plants.end(), plant) != plants.end();
    }
};

// Mock Inventory that provides a storage group
class MockInventory {
    std::shared_ptr<MockGroup> storage;
public:
    MockInventory() { storage = std::make_shared<MockGroup>(42); }
    std::shared_ptr<MockGroup> getStorageGroup() { return storage; }
};

// ---------------------------------------------------------
// Adapter functions so AddToStorageCommand sees correct types
// ---------------------------------------------------------
namespace {
    struct PlantAdapter : Plant {
        std::shared_ptr<MockPlant> impl;
        explicit PlantAdapter(std::shared_ptr<MockPlant> impl) : impl(std::move(impl)) {}
        uint64_t getId() const { return impl->getId(); }
    };

    struct InventoryAdapter : Inventory {
        std::shared_ptr<MockInventory> impl;
        explicit InventoryAdapter(std::shared_ptr<MockInventory> impl) : impl(std::move(impl)) {}
        std::shared_ptr<Group> findGroupByName(const std::string&) { return nullptr; }
    };
}

// ---------------------------------------------------------
// Tests
// ---------------------------------------------------------

TEST_CASE("AddToStorageCommand - Constructor initializes correctly") {
    auto mockPlant = std::make_shared<MockPlant>(5);
    auto mockInventory = std::make_shared<MockInventory>();

    auto plant = std::make_shared<PlantAdapter>(mockPlant);
    auto inventory = std::make_shared<InventoryAdapter>(mockInventory);

    AddToStorageCommand cmd(plant, inventory);

    CHECK(cmd.getTargetId() == plant->getId());
    CHECK(cmd.getStatus() == Command::Status::Pending);
}

TEST_CASE("AddToStorageCommand - Execute fails with null pointers") {
    auto plant = std::make_shared<PlantAdapter>(std::make_shared<MockPlant>(2));
    auto inventory = std::make_shared<InventoryAdapter>(std::make_shared<MockInventory>());

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

TEST_CASE("AddToStorageCommand - Status setters and getters work correctly") {
    auto plant = std::make_shared<PlantAdapter>(std::make_shared<MockPlant>(3));
    auto inventory = std::make_shared<InventoryAdapter>(std::make_shared<MockInventory>());

    AddToStorageCommand cmd(plant, inventory);

    cmd.setStatus(Command::Status::Failed);
    CHECK(cmd.getStatus() == Command::Status::Failed);

    cmd.setStatus(Command::Status::Completed);
    CHECK(cmd.getStatus() == Command::Status::Completed);
}

TEST_CASE("AddToStorageCommand - Target ID setters and getters work correctly") {
    auto plant = std::make_shared<PlantAdapter>(std::make_shared<MockPlant>(3));
    auto inventory = std::make_shared<InventoryAdapter>(std::make_shared<MockInventory>());

    AddToStorageCommand cmd(plant, inventory);

    cmd.setTargetId(12345);
    CHECK(cmd.getTargetId() == 12345);
}
