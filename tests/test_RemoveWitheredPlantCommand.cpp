#include "../include/doctest.h"

#include "../include/Components/Group.h"
#include "../include/Components/Rose.h"
#include "../include/Patterns/Command/RemoveWitheredPlantCommand.h"

TEST_CASE("RemoveWitheredPlantCommand removes plant with health <= 0") {
    auto group = std::make_shared<Group>("Plot1");
    auto plant = std::make_shared<Rose>();
    
    group->add(plant);
    plant->setHealth(0);
    
    RemoveWitheredPlantCommand cmd(plant, group);
    cmd.execute();
    
    CHECK_EQ(cmd.getStatus(), Command::Status::Completed);
    
    auto members = group->members();
    CHECK_EQ(members.size(), 0);
}

TEST_CASE("RemoveWitheredPlantCommand fails when plant health > 0") {
    auto group = std::make_shared<Group>("Plot1");
    auto plant = std::make_shared<Rose>();
    
    group->add(plant);
    plant->setHealth(50);
    
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
    plant->setHealth(0);
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
