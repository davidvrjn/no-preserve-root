#pragma once

#include <atomic>
#include <memory>

#include "../../Utils/CommandLog.h"
#include "Command.h"

class LoggingCommand : public Command {
   public:
    LoggingCommand(std::unique_ptr<Command> inner, const std::shared_ptr<CommandLog>& log,
                   int step);
    ~LoggingCommand() override = default;

    void execute() override;

    Status getStatus() const override;
    void setStatus(Status s) override;
    uint64_t getTargetId() const override;
    void setTargetId(uint64_t id) override;

    std::string toString() const override;

    // Accessor for the wrapped command (needed for type checking in cleanup logic)
    Command* getInnerCommand() const { return inner_.get(); }

    // Extract the inner command (transfers ownership)
    std::unique_ptr<Command> extractInnerCommand() { return std::move(inner_); }

    // The step in which this command is actually executed. This is set by the
    // caller (Nursery) just before execution so the CommandLog can record the
    // executed step separately from the queued step.
    void setExecutedStep(int step) { executedStep_ = step; }
    int getExecutedStep() const { return executedStep_; }

   private:
    std::unique_ptr<Command> inner_;
    std::shared_ptr<CommandLog> log_;
    int stepQueued_;
    int executedStep_ = -1;
    uint64_t id_;
    static std::atomic<uint64_t> nextId_;
};
