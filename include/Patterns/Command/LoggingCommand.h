#pragma once

#include "Command.h"
#include "../../Utils/CommandLog.h"

#include <memory>
#include <atomic>

class LoggingCommand : public Command {
public:
    LoggingCommand(std::unique_ptr<Command> inner, const std::shared_ptr<CommandLog>& log, int step);
    ~LoggingCommand() override = default;

    void execute() override;

    Status getStatus() const override;
    void setStatus(Status s) override;
    uint64_t getTargetId() const override;
    void setTargetId(uint64_t id) override;

    std::string toString() const override;

    // Accessor for the wrapped command (needed for type checking in cleanup logic)
    Command* getInnerCommand() const { return inner_.get(); }

private:
    std::unique_ptr<Command> inner_; 
    std::shared_ptr<CommandLog> log_;
    int stepQueued_;
    uint64_t id_;
    static std::atomic<uint64_t> nextId_;
};
