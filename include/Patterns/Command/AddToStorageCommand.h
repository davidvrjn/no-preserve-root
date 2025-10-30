#pragma once
// sub out the includes later
#include <memory>

#include "Command.h"
#include "InventoryComponent.h"
#include "Plant.h"

class AddToStorageCommand : public Command {
   private:
    std::shared_ptr<InventoryComponent> source_;
    std::shared_ptr<InventoryComponent> target_;
    std::shared_ptr<Plant> plant_;

   public:
    AddToStorageCommand(const std::shared_ptr<InventoryComponent>& source,
                        const std::shared_ptr<InventoryComponent>& target,
                        const std::shared_ptr<Plant>& plant);

    void execute() override;
    Status getStatus() const override;
    void setStatus(Status s) override;
    uint64_t getTargetId() const override;
    void setTargetId(uint64_t id) override;
};