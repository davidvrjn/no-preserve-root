#include <algorithm>
#include <memory>
#include <vector>

#include "../../include/Patterns/Command/AddToStorageCommand.h"
#include "../include/doctest.h"

class DummyPlant : public Plant {
public:
    uint64_t id;
    explicit DummyPlant(uint64_t id) : id(id) {}
    uint64_t getId() const { return id; }
};

class DummyInventoryComponent : public InventoryComponent {
public:
    uint64_t id;
    explicit DummyInventoryComponent(uint64_t id) : id(id) {}
    uint64_t getId() const override { return id; }
};

class DummyGroup : public Group {
public:
    uint64_t id;
    std::vector<std::shared_ptr<Plant>> plants;

    explicit DummyGroup(uint64_t id) : id(id) {}

    uint64_t getId() const override { return id; }

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

// ---------------- Test Cases ----------------

TEST_CASE("AddToStorageCommand - Constructor initializes correctly") {
    auto source = std::make_shared<DummyInventoryComponent>(1);
    auto target = std::make_shared<DummyInventoryComponent>(42);
    auto plant  = std::make_shared<DummyPlant>(5);

    AddToStorageCommand cmd(source, target, plant);

    CHECK(cmd.getTargetId() == 42);
    CHECK(cmd.getStatus() == Command::Status::Pending);
}

TEST_CASE("AddToStorageCommand - Execute fails with null pointers") {
    SUBCASE("Null source") {
        auto target = std::make_shared<DummyInventoryComponent>(1);
        auto plant  = std::make_shared<DummyPlant>(2);
        AddToStorageCommand cmd(nullptr, target, plant);
        cmd.execute();
        CHECK(cmd.getStatus() == Command::Status::Failed);
    }

    SUBCASE("Null target") {
        auto source = std::make_shared<DummyInventoryComponent>(1);
        auto plant  = std::make_shared<DummyPlant>(2);
        AddToStorageCommand cmd(source, nullptr, plant);
        cmd.execute();
        CHECK(cmd.getStatus() == Command::Status::Failed);
    }

    SUBCASE("Null plant") {
        auto source = std::make_shared<DummyInventoryComponent>(1);
        auto target = std::make_shared<DummyInventoryComponent>(2);
        AddToStorageCommand cmd(source, target, nullptr);
        cmd.execute();
        CHECK(cmd.getStatus() == Command::Status::Failed);
    }
}

TEST_CASE("AddToStorageCommand - Execute fails when source is not a Group") {
    auto source = std::make_shared<DummyInventoryComponent>(1);
    auto target = std::make_shared<DummyInventoryComponent>(2);
    auto plant  = std::make_shared<DummyPlant>(3);

    AddToStorageCommand cmd(source, target, plant);
    cmd.execute();

    CHECK(cmd.getStatus() == Command::Status::Failed);
}

TEST_CASE("AddToStorageCommand - Execute fails when target is not a Group") {
    auto source = std::make_shared<DummyGroup>(1);
    auto target = std::make_shared<DummyInventoryComponent>(2);
    auto plant  = std::make_shared<DummyPlant>(3);

    source->add(plant);

    AddToStorageCommand cmd(source, target, plant);
    cmd.execute();

    CHECK(cmd.getStatus() == Command::Status::Failed);
    CHECK(source->contains(plant));  // Plant should still be in source
}

TEST_CASE("AddToStorageCommand - Successful execution moves plant to target") {
    auto source = std::make_shared<DummyGroup>(1);
    auto target = std::make_shared<DummyGroup>(2);
    auto plant  = std::make_shared<DummyPlant>(3);

    source->add(plant);
    CHECK(source->contains(plant));
    CHECK_FALSE(target->contains(plant));

    AddToStorageCommand cmd(source, target, plant);
    cmd.execute();

    CHECK(cmd.getStatus() == Command::Status::Completed);
    CHECK_FALSE(source->contains(plant));
    CHECK(target->contains(plant));
}

TEST_CASE("AddToStorageCommand - Status setters and getters work correctly") {
    auto source = std::make_shared<DummyInventoryComponent>(1);
    auto target = std::make_shared<DummyInventoryComponent>(2);
    auto plant  = std::make_shared<DummyPlant>(3);

    AddToStorageCommand cmd(source, target, plant);

    cmd.setStatus(Command::Status::Failed);
    CHECK(cmd.getStatus() == Command::Status::Failed);

    cmd.setStatus(Command::Status::Completed);
    CHECK(cmd.getStatus() == Command::Status::Completed);
}

TEST_CASE("AddToStorageCommand - Target ID setters and getters work correctly") {
    auto source = std::make_shared<DummyInventoryComponent>(1);
    auto target = std::make_shared<DummyInventoryComponent>(2);
    auto plant  = std::make_shared<DummyPlant>(3);

    AddToStorageCommand cmd(source, target, plant);

    cmd.setTargetId(12345);
    CHECK(cmd.getTargetId() == 12345);
}
