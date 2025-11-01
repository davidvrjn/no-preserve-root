#include "../../../include/Patterns/Command/LoggingCommand.h"

#include <chrono>
#include <ctime>
#include <fstream>

std::atomic<uint64_t> LoggingCommand::nextId_{1};

LoggingCommand::LoggingCommand(std::unique_ptr<Command> inner, const std::shared_ptr<CommandLog>& log, int step)
    : inner_(std::move(inner)), log_(log), stepQueued_(step), id_(nextId_.fetch_add(1))
{
    if (log_ && inner_) {
        CommandLogEntry e;
        e.phase = CommandLogEntry::Phase::Pending;
        e.step = stepQueued_;
        e.id = id_;
        e.text = inner_->toString();
        e.timestamp = static_cast<long>(std::time(nullptr));
        log_->append(e);
    }
}

void LoggingCommand::execute() {
    if (!inner_) return;
    
    // DEBUG: Log execution
    std::ofstream debugLog("debug.log", std::ios::app);
    debugLog << "   [LoggingCommand] Executing command ID " << id_ << ": " << inner_->toString() << "\n";
    debugLog.close();
    
    inner_->execute();

    if (!log_) return;

    CommandLogEntry e;
    e.step = stepQueued_;
    e.executedStep = executedStep_;
    e.id = id_;
    e.text = inner_->toString();
    e.timestamp = static_cast<long>(std::time(nullptr));
    if (inner_->getStatus() == Status::Completed) e.phase = CommandLogEntry::Phase::Completed;
    else if (inner_->getStatus() == Status::Failed) e.phase = CommandLogEntry::Phase::Failed;
    else e.phase = CommandLogEntry::Phase::Completed; // default to completed for other states

    // DEBUG: Log result
    std::ofstream debugLog2("debug.log", std::ios::app);
    debugLog2 << "   [LoggingCommand] Logged result for ID " << id_ << " as " 
              << (e.phase == CommandLogEntry::Phase::Completed ? "COMPLETED" : 
                  e.phase == CommandLogEntry::Phase::Failed ? "FAILED" : "UNKNOWN") << "\n";
    debugLog2.close();
    
    log_->append(e);
}

Command::Status LoggingCommand::getStatus() const { return inner_ ? inner_->getStatus() : Status::Failed; }
void LoggingCommand::setStatus(Status s) { if (inner_) inner_->setStatus(s); }
uint64_t LoggingCommand::getTargetId() const { return inner_ ? inner_->getTargetId() : 0; }
void LoggingCommand::setTargetId(uint64_t id) { if (inner_) inner_->setTargetId(id); }

std::string LoggingCommand::toString() const { return inner_ ? inner_->toString() : std::string("<null>"); }
