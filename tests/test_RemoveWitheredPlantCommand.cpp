#include "../include/Components/Group.h"
#include "../include/Components/Rose.h"
#include "../include/Patterns/Command/RemoveWitheredPlantCommand.h"
#include "../include/Patterns/State/Seedling.h"
#include "../include/Patterns/State/Withered.h"
#include "../include/doctest.h"

TEST_CASE("RemoveWitheredPlantCommand removes plant in Withered state") {
    auto group = std::make_shared<Group>("Plot1");
    auto plant = std::make_shared<Rose>();

    group->add(plant);
    plant->setState(std::make_unique<Withered>());

    RemoveWitheredPlantCommand cmd(plant, group);
    cmd.execute();

    CHECK_EQ(cmd.getStatus(), Command::Status::Completed);

    auto members = group->members();
    CHECK_EQ(members.size(), 0);
}

TEST_CASE("RemoveWitheredPlantCommand fails when plant not in Withered state") {
    auto group = std::make_shared<Group>("Plot1");
    auto plant = std::make_shared<Rose>();

    group->add(plant);
    plant->setState(std::make_unique<Seedling>());

    RemoveWitheredPlantCommand cmd(plant, group);
    cmd.execute();

    CHECK_EQ(cmd.getStatus(), Command::Status::Failed);

    auto members = group->members();
    CHECK_EQ(members.size(), 1);
}

TEST_CASE("RemoveWitheredPlantCommand fails when plant pointer is null") {
    auto group = std::make_shared<Group>("Plot1");
    std::shared_ptr<Rose> nullPlant = nullptr;

    RemoveWitheredPlantCommand cmd(nullPlant, group);
    cmd.execute();

    CHECK_EQ(cmd.getStatus(), Command::Status::Failed);
}

TEST_CASE("RemoveWitheredPlantCommand fails when group pointer is null") {
    std::shared_ptr<Rose> plant = std::make_shared<Rose>();
    plant->setState(std::make_unique<Withered>());
    std::shared_ptr<Group> nullGroup = nullptr;

    RemoveWitheredPlantCommand cmd(plant, nullGroup);
    cmd.execute();

    CHECK_EQ(cmd.getStatus(), Command::Status::Failed);
}

TEST_CASE("RemoveWitheredPlantCommand stores correct target ID") {
    auto group = std::make_shared<Group>("Plot1");
    auto plant = std::make_shared<Rose>();

    uint64_t plantId = plant->getId();

    RemoveWitheredPlantCommand cmd(plant, group);

    CHECK_EQ(cmd.getTargetId(), plantId);
}

TEST_CASE("RemoveWitheredPlantCommand can set and get status") {
    auto group = std::make_shared<Group>("Plot1");
    auto plant = std::make_shared<Rose>();

    RemoveWitheredPlantCommand cmd(plant, group);

    CHECK_EQ(cmd.getStatus(), Command::Status::Pending);

    cmd.setStatus(Command::Status::Cancelled);
    CHECK_EQ(cmd.getStatus(), Command::Status::Cancelled);
}

TEST_CASE("RemoveWitheredPlantCommand can set target ID") {
    auto group = std::make_shared<Group>("Plot1");
    auto plant = std::make_shared<Rose>();

    RemoveWitheredPlantCommand cmd(plant, group);

    cmd.setTargetId(12345);
    CHECK_EQ(cmd.getTargetId(), 12345);
}
