#pragma once

#include <cstdint>
#include <mutex>
#include <string>
#include <vector>

struct CommandLogEntry {
    enum class Phase { Pending, Completed, Failed } phase;
    int step;          // step when command was queued
    int executedStep = -1;  // step when command was executed (or -1 if not executed yet)
    uint64_t id;       // unique id for the command instance
    std::string text;  // human readable description
    long timestamp;    // time_t
};

class CommandLog {
   public:
    CommandLog();

    void append(const CommandLogEntry& e);

    // Return all entries for a given step (by queued step)
    std::vector<CommandLogEntry> entriesForStep(int step) const;

    // Convenience getters
    std::vector<std::string> completedTextsForStep(int step) const;          // by queued step
    std::vector<std::string> completedTextsForExecutedStep(int step) const;  // by executed step
    // Returns pending texts for the step that do NOT have a completed/failed entry
    std::vector<std::string> remainingPendingTextsForStep(int step) const;

    // Clear all entries (e.g., at start of new day)
    void clear();

   private:
    mutable std::mutex mu;
    std::vector<CommandLogEntry> entries;
};
