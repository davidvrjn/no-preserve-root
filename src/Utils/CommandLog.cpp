#include "../../include/Utils/CommandLog.h"

#include <algorithm>
#include <chrono>

CommandLog::CommandLog() {}

void CommandLog::append(const CommandLogEntry &e) {
    std::lock_guard<std::mutex> lk(mu);
    entries.push_back(e);
}

std::vector<CommandLogEntry> CommandLog::entriesForStep(int step) const {
    std::lock_guard<std::mutex> lk(mu);
    std::vector<CommandLogEntry> out;
    for (const auto &e : entries) {
        if (e.step == step) out.push_back(e);
    }
    return out;
}

std::vector<std::string> CommandLog::completedTextsForStep(int step) const {
    auto es = entriesForStep(step);
    std::vector<std::string> out;
    for (const auto &e : es) {
        // Show both completed and failed commands (failed = customers left unfulfilled)
        if (e.phase == CommandLogEntry::Phase::Completed) {
            out.push_back(e.text);
        } else if (e.phase == CommandLogEntry::Phase::Failed) {
            out.push_back(e.text + " (FAILED)");
        }
    }
    return out;
}

std::vector<std::string> CommandLog::completedTextsForExecutedStep(int step) const {
    std::lock_guard<std::mutex> lk(mu);
    std::vector<std::string> out;
    for (const auto &e : entries) {
        // Find commands that were executed (completed or failed) during this step
        if (e.executedStep == step) {
            if (e.phase == CommandLogEntry::Phase::Completed) {
                out.push_back(e.text);
            } else if (e.phase == CommandLogEntry::Phase::Failed) {
                out.push_back(e.text + " (FAILED)");
            }
        }
    }
    return out;
}

std::vector<std::string> CommandLog::remainingPendingTextsForStep(int step) const {
    auto es = entriesForStep(step);
    // collect ids of completed/failed
    std::vector<uint64_t> doneIds;
    for (const auto &e : es) {
        if (e.phase == CommandLogEntry::Phase::Completed ||
            e.phase == CommandLogEntry::Phase::Failed) {
            doneIds.push_back(e.id);
        }
    }

    std::vector<std::string> out;
    for (const auto &e : es) {
        if (e.phase == CommandLogEntry::Phase::Pending) {
            bool done = false;
            for (auto id : doneIds)
                if (id == e.id) {
                    done = true;
                    break;
                }
            if (!done) out.push_back(e.text);
        }
    }
    return out;
}

void CommandLog::clear() {
    std::lock_guard<std::mutex> lk(mu);
    entries.clear();
}
