#include "../include/doctest.h"

#include "../include/Patterns/Command/FulfillCustomerCommand.h"
#include "../include/Patterns/Builder/PlantSpecification.h"
#include "../include/Core/Inventory.h"
#include "../include/Actors/Customer.h"
#include "../include/Components/Rose.h"
#include "../include/Components/Cactus.h"
#include "../include/Patterns/Decorator/GiftWrapDecorator.h"
#include "../include/Patterns/Decorator/PotDecorator.h"
#include "../include/Patterns/Decorator/RibbonDecorator.h"

namespace doctest {
    template <>
    struct StringMaker<std::shared_ptr<InventoryComponent>> {
        static String convert(const std::shared_ptr<InventoryComponent>& comp) {
            if (!comp) return "nullptr";
            return comp->getName().c_str();
        }
    };
}

TEST_CASE("FulfillCustomerCommand RECOMMENDATION succeeds when matching plant exists") {
    auto inventory = std::make_shared<Inventory>();
    auto customer = std::make_shared<Customer>();

    auto cactus = std::make_shared<Cactus>();
    inventory->add(cactus);

    auto spec = std::make_unique<PlantSpecification>();
    spec->requestType = RequestType::RECOMMENDATION;
    spec->waterReq = cactus->getWaterRequirement();
    spec->seasonReq = cactus->getPreferredSeasons().front();

    FulfillCustomerCommand cmd(std::move(spec), inventory, customer);
    cmd.execute();

    CHECK_EQ(cmd.getStatus(), Command::Status::Completed);
    CHECK_NE(cmd.getTargetId(), 0);
}

TEST_CASE("FulfillCustomerCommand RECOMMENDATION fails when no match exists") {
    auto inventory = std::make_shared<Inventory>();
    auto customer = std::make_shared<Customer>();

    auto rose = std::make_shared<Rose>();
    inventory->add(rose);

    auto spec = std::make_unique<PlantSpecification>();
    spec->requestType = RequestType::RECOMMENDATION;
    spec->waterReq = WaterRequirement::VERY_LOW; 
    spec->seasonReq = Season::WINTER;           

    FulfillCustomerCommand cmd(std::move(spec), inventory, customer);
    cmd.execute();

    CHECK_EQ(cmd.getStatus(), Command::Status::Failed);
}

TEST_CASE("FulfillCustomerCommand PURCHASE succeeds with decorators") {
    auto inventory = std::make_shared<Inventory>();
    auto customer = std::make_shared<Customer>();

    auto rose = std::make_shared<Rose>();
    inventory->add(rose);

    auto spec = std::make_unique<PlantSpecification>();
    spec->requestType = RequestType::PURCHASE;
    spec->explicitName = "Rose";
    spec->decorators = {"GiftWrap", "Pot", "Ribbon"};

    FulfillCustomerCommand cmd(std::move(spec), inventory, customer);
    cmd.execute();

    CHECK_EQ(cmd.getStatus(), Command::Status::Completed);
    CHECK_EQ(cmd.getTargetId(), rose->getId());
}

TEST_CASE("FulfillCustomerCommand PURCHASE fails if plant not in inventory") {
    auto inventory = std::make_shared<Inventory>();
    auto customer = std::make_shared<Customer>();

    auto spec = std::make_unique<PlantSpecification>();
    spec->requestType = RequestType::PURCHASE;
    spec->explicitName = "NonExistentPlant";

    FulfillCustomerCommand cmd(std::move(spec), inventory, customer);
    cmd.execute();

    CHECK_EQ(cmd.getStatus(), Command::Status::Failed);
}