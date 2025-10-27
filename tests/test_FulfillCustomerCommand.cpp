#include "../include/Actors/Customer.h"
#include "../include/Components/Cactus.h"
#include "../include/Components/Group.h"
#include "../include/Components/Rose.h"
#include "../include/Core/Inventory.h"
#include "../include/Patterns/Builder/PlantSpecification.h"
#include "../include/Patterns/Command/FulfillCustomerCommand.h"
#include "../include/Patterns/Decorator/GiftWrapDecorator.h"
#include "../include/Patterns/Decorator/PotDecorator.h"
#include "../include/Patterns/Decorator/RibbonDecorator.h"
#include "../include/Patterns/Iterator/Iterator.h"
#include "../include/doctest.h"

namespace doctest {
template <>
struct StringMaker<std::shared_ptr<InventoryComponent>> {
    static String convert(const std::shared_ptr<InventoryComponent>& comp) {
        if (!comp) return "nullptr";
        return comp->getName().c_str();
    }
};
}  // namespace doctest

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

TEST_CASE("FulfillCustomerCommand PURCHASE succeeds from Storage with decorators") {
    auto inventory = std::make_shared<Inventory>();
    auto customer = std::make_shared<Customer>();

    // Create a Storage group and add a Rose
    auto storage = std::make_shared<Group>("Storage");
    auto rose = std::make_shared<Rose>();
    storage->add(rose);
    inventory->add(storage);

    auto spec = std::make_unique<PlantSpecification>();
    spec->requestType = RequestType::PURCHASE;
    spec->explicitName = "Rose";
    spec->decorators = {"GiftWrap", "Pot", "Ribbon"};

    FulfillCustomerCommand cmd(std::move(spec), inventory, customer);
    cmd.execute();

    CHECK_EQ(cmd.getStatus(), Command::Status::Completed);
    CHECK_EQ(cmd.getTargetId(), rose->getId());

    auto decorated = cmd.getDecoratedPlant();
    CHECK(decorated != nullptr);
    CHECK(decorated->getName().find("Rose") != std::string::npos);
    CHECK(decorated->getName().find("Gift Wrap") != std::string::npos);
    CHECK(decorated->getName().find("Pot") != std::string::npos);
    CHECK(decorated->getName().find("Ribbon") != std::string::npos);

    CHECK_GT(cmd.getSalePrice(), rose->getPrice());

    // Plant should be removed from storage
    auto it = storage->createIterator();
    bool roseStillThere = false;
    while (it->hasNext()) {
        if (it->next() == rose) {
            roseStillThere = true;
            break;
        }
    }
    CHECK_FALSE(roseStillThere);
}

TEST_CASE("FulfillCustomerCommand PURCHASE fails if plant not in Storage") {
    auto inventory = std::make_shared<Inventory>();
    auto customer = std::make_shared<Customer>();

    // Plant exists but in a normal group, not Storage
    auto plot = std::make_shared<Group>("Plot1");
    auto rose = std::make_shared<Rose>();
    plot->add(rose);
    inventory->add(plot);

    auto spec = std::make_unique<PlantSpecification>();
    spec->requestType = RequestType::PURCHASE;
    spec->explicitName = "Rose";

    FulfillCustomerCommand cmd(std::move(spec), inventory, customer);
    cmd.execute();

    CHECK_EQ(cmd.getStatus(), Command::Status::Failed);
}