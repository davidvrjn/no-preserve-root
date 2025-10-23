#include "../include/doctest.h"

#include "../include/Components/Rose.h"
#include "../include/Components/Cactus.h"
#include "../include/Patterns/Decorator/GiftWrapDecorator.h"
#include "../include/Patterns/Decorator/PotDecorator.h"
#include "../include/Patterns/Decorator/RibbonDecorator.h"

// Custom printer for doctest so shared_ptr<InventoryComponent> shows something useful
namespace doctest {
    template <>
    struct StringMaker<std::shared_ptr<InventoryComponent>> {
        static String convert(const std::shared_ptr<InventoryComponent>& comp) {
            if (!comp) return "nullptr";
            return comp->getName().c_str();  
        }
    };
}

TEST_CASE("GiftWrapDecorator modifies name, price, typeName, and cloning") {
    auto rose = std::make_shared<Rose>();
    auto wrapped = std::make_shared<GiftWrapDecorator>(rose);

    CHECK_EQ(wrapped->getName(), "Rose + Gift Wrap");
    CHECK_GT(wrapped->getPrice(), rose->getPrice());
    CHECK_EQ(wrapped->typeName(), "GiftWrapDecorator");

    SUBCASE("clone preserves state") {
        auto clone = wrapped->clone();
        CHECK(clone != nullptr);
        CHECK_EQ(clone->getName(), wrapped->getName());
        CHECK_EQ(clone->getPrice(), wrapped->getPrice());
        CHECK_EQ(clone->typeName(), "GiftWrapDecorator");
    }

    SUBCASE("blueprintClone creates a new decorated plant") {
        auto fresh = wrapped->blueprintClone();
        CHECK(fresh != nullptr);
        CHECK_EQ(fresh->typeName(), "GiftWrapDecorator");
        CHECK_EQ(fresh->getName(), wrapped->getName());
    }
}

TEST_CASE("PotDecorator modifies name, price, typeName, and cloning") {
    auto cactus = std::make_shared<Cactus>();
    auto potted = std::make_shared<PotDecorator>(cactus);

    CHECK_EQ(potted->getName(), "Cactus in Pot");
    CHECK_GT(potted->getPrice(), cactus->getPrice());
    CHECK_EQ(potted->typeName(), "PotDecorator");

    SUBCASE("clone preserves state") {
        auto clone = potted->clone();
        CHECK(clone != nullptr);
        CHECK_EQ(clone->getName(), potted->getName());
        CHECK_EQ(clone->getPrice(), potted->getPrice());
        CHECK_EQ(clone->typeName(), "PotDecorator");
    }

    SUBCASE("blueprintClone creates a new decorated plant") {
        auto fresh = potted->blueprintClone();
        CHECK(fresh != nullptr);
        CHECK_EQ(fresh->typeName(), "PotDecorator");
        CHECK_EQ(fresh->getName(), potted->getName());
    }
}

TEST_CASE("RibbonDecorator modifies name, price, typeName, and cloning") {
    auto rose = std::make_shared<Rose>();
    auto ribboned = std::make_shared<RibbonDecorator>(rose);

    CHECK_EQ(ribboned->getName(), "Rose with Ribbon");
    CHECK_GT(ribboned->getPrice(), rose->getPrice());
    CHECK_EQ(ribboned->typeName(), "RibbonDecorator");

    SUBCASE("clone preserves state") {
        auto clone = ribboned->clone();
        CHECK(clone != nullptr);
        CHECK_EQ(clone->getName(), ribboned->getName());
        CHECK_EQ(clone->getPrice(), ribboned->getPrice());
        CHECK_EQ(clone->typeName(), "RibbonDecorator");
    }

    SUBCASE("blueprintClone creates a new decorated plant") {
        auto fresh = ribboned->blueprintClone();
        CHECK(fresh != nullptr);
        CHECK_EQ(fresh->typeName(), "RibbonDecorator");
        CHECK_EQ(fresh->getName(), ribboned->getName());
    }
}

TEST_CASE("Multiple decorators can be chained together") {
    auto rose = std::make_shared<Rose>();
    auto decorated = std::make_shared<RibbonDecorator>(
                        std::make_shared<PotDecorator>(
                            std::make_shared<GiftWrapDecorator>(rose)));

    auto name = decorated->getName();
    CHECK(name.find("Rose") != std::string::npos);
    CHECK(name.find("Gift Wrap") != std::string::npos);
    CHECK(name.find("Pot") != std::string::npos);
    CHECK(name.find("Ribbon") != std::string::npos);

    CHECK_GT(decorated->getPrice(), rose->getPrice());

    SUBCASE("clone preserves full decorator chain") {
        auto clone = decorated->clone();
        CHECK(clone != nullptr);
        CHECK_EQ(clone->getName(), decorated->getName());
        CHECK_EQ(clone->getPrice(), decorated->getPrice());
    }

    SUBCASE("blueprintClone creates fresh decorated chain") {
        auto fresh = decorated->blueprintClone();
        CHECK(fresh != nullptr);
        CHECK_EQ(fresh->getName(), decorated->getName());
        CHECK_GT(fresh->getPrice(), rose->getPrice());
    }
}