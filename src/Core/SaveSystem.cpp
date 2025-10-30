#include "../../include/Core/SaveSystem.h"

#include <fstream>
#include <iostream>

#include "../../include/Core/Nursery.h"
#include "../../include/Patterns/Memento/Memento.h"
#include "../../include/json.hpp"

SaveSystem::SaveSystem() = default;

void SaveSystem::save(const std::shared_ptr<Nursery>& nursery, const std::string& filename) {
    if (!nursery) {
        std::cerr << "SaveSystem::save - Nursery is null\n";
        return;
    }

    try {
        // Create memento from nursery
        Memento* memento = nursery->createMemento();

        if (!memento) {
            std::cerr << "SaveSystem::save - Failed to create memento\n";
            return;
        }

        // Open file for writing
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "SaveSystem::save - Failed to open file: " << filename << "\n";
            delete memento;
            return;
        }

        // Parse and pretty-print JSON for readability
        auto json = nlohmann::json::parse(memento->getState().serializedData);
        file << json.dump(2);  // 2-space indentation

        file.close();
        delete memento;

        std::cout << "Game saved to: " << filename << " (Day " << json["currentDay"] << ")\n";

    } catch (const std::exception& e) {
        std::cerr << "SaveSystem::save - Error: " << e.what() << "\n";
    }
}

std::unique_ptr<Memento> SaveSystem::load(const std::string& filename) {
    try {
        // Open file for reading
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "SaveSystem::load - Failed to open file: " << filename << "\n";
            return nullptr;
        }

        // Read entire file
        std::string jsonData((std::istreambuf_iterator<char>(file)),
                             std::istreambuf_iterator<char>());
        file.close();

        // Parse JSON to validate and extract day
        auto json = nlohmann::json::parse(jsonData);
        int day = json["currentDay"].get<int>();

        // Create memento
        Memento::NurseryState state;
        state.day = day;
        state.serializedData = jsonData;

        std::cout << "Game loaded from: " << filename << " (Day " << day << ")\n";

        return std::make_unique<Memento>(state);

    } catch (const std::exception& e) {
        std::cerr << "SaveSystem::load - Error: " << e.what() << "\n";
        return nullptr;
    }
}

bool SaveSystem::fileExists(const std::string& filename) {
    std::ifstream file(filename);
    return file.good();
}

bool SaveSystem::validateSaveFile(const std::string& filename) {
    try {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return false;
        }

        std::string jsonData((std::istreambuf_iterator<char>(file)),
                             std::istreambuf_iterator<char>());
        file.close();

        // Try to parse - assign result to actually use it
        auto parsed = nlohmann::json::parse(jsonData);
        return !parsed.is_null();

    } catch (...) {
        return false;
    }
}

int SaveSystem::getSaveDay(const std::string& filename) {
    try {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return -1;
        }

        std::string jsonData((std::istreambuf_iterator<char>(file)),
                             std::istreambuf_iterator<char>());
        file.close();

        auto json = nlohmann::json::parse(jsonData);
        return json["currentDay"].get<int>();

    } catch (...) {
        return -1;
    }
}

std::string SaveSystem::prettyPrintSave(const std::string& filename) {
    try {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return "Error: Could not open file: " + filename;
        }

        std::string jsonData((std::istreambuf_iterator<char>(file)),
                             std::istreambuf_iterator<char>());
        file.close();

        auto json = nlohmann::json::parse(jsonData);
        return json.dump(2);

    } catch (const std::exception& e) {
        return "Error: " + std::string(e.what());
    }
}
