#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <mutex>

struct CommandLogEntry {
    enum class Phase { Pending, Completed, Failed } phase;
    int step;             // step when command was queued
    uint64_t id;          // unique id for the command instance
    std::string text;     // human readable description
    long timestamp;       // time_t
};

class CommandLog {
public:
    CommandLog();

    void append(const CommandLogEntry& e);

    // Return all entries for a given step
    std::vector<CommandLogEntry> entriesForStep(int step) const;

    // Convenience getters
    std::vector<std::string> completedTextsForStep(int step) const;
    // Returns pending texts for the step that do NOT have a completed/failed entry
    std::vector<std::string> remainingPendingTextsForStep(int step) const;

private:
    mutable std::mutex mu;
    std::vector<CommandLogEntry> entries;
};
