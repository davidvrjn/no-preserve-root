#include <algorithm>
#include <memory>
#include <vector>

#include "../../include/Patterns/Command/AddToStorageCommand.h"
#include "../include/doctest.h"

class DummyPant : public Plant {
   public:
    uint64_t id;
    explicit DummyPant(uint64_t id) : id(id) {}
    uint64_t getId() const { return id; }
}

class DummyInventoryComponent : public InventoryComponent {
   public:
    uint64_t id;
    explicit DummyInventoryComponent(uint64_t id) : id(id) {}
    uint64_t getId() const override { return id; }
}

class DummyGroup : public Group {
   public:
    uint64_t id;
    std::vector<std::shared_ptr<Plant>> plants;

    explicit DummyGroup(uint64_t id) : id(id) {}

    uint64_t getId() const override { return id; }

    void add(const std::shared_ptr<Plant>& plant) override { plants.push_back(plant); }

    void remove(const std::shared_ptr<Plant>& plant) override {
        plants.erase(std::remove(plants.begin(), plants.end(), plant), plants.end());
    }

    bool contains(const std::shared_ptr<Plant>& plant) const {
        return std::find(plants.begin(), plants.end(), plant) != plants.end();
    }
}