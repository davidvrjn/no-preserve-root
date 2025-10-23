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
            return comp->getName().c_str();  // Show the decorated name
        }
    };
}

TEST_CASE("GiftWrapDecorator modifies name, price, and typeName") {
    auto rose = std::make_shared<Rose>();
    auto wrapped = std::make_shared<GiftWrapDecorator>(rose);

    CHECK_EQ(wrapped->getName(), "Rose + Gift Wrap");
    CHECK_GT(wrapped->getPrice(), rose->getPrice());
    CHECK_EQ(wrapped->typeName(), "GiftWrapDecorator");

    SUBCASE("blueprintClone creates a new decorated plant") {
        auto clone = wrapped->blueprintClone();
        CHECK(clone.get() != nullptr);
        CHECK_EQ(clone->typeName(), "GiftWrapDecorator");
    }

    SUBCASE("serialize adds prefix and deserialize restores state") {
        auto data = wrapped->serialize();
        CHECK(data.find("GiftWrap|") == 0);

        auto freshRose = std::make_shared<Rose>();
        auto deserialized = std::make_shared<GiftWrapDecorator>(freshRose);
        deserialized->deserialize(data);

        CHECK_EQ(deserialized->typeName(), "GiftWrapDecorator");
    }
}

TEST_CASE("PotDecorator modifies name, price, and typeName") {
    auto cactus = std::make_shared<Cactus>();
    auto potted = std::make_shared<PotDecorator>(cactus);

    CHECK_EQ(potted->getName(), "Cactus in Pot");
    CHECK_GT(potted->getPrice(), cactus->getPrice());
    CHECK_EQ(potted->typeName(), "PotDecorator");

    SUBCASE("blueprintClone creates a new decorated plant") {
        auto clone = potted->blueprintClone();
        CHECK(clone.get() != nullptr);
        CHECK_EQ(clone->typeName(), "PotDecorator");
    }

    SUBCASE("serialize adds prefix and deserialize restores state") {
        auto data = potted->serialize();
        CHECK(data.find("Pot|") == 0);

        auto freshCactus = std::make_shared<Cactus>();
        auto deserialized = std::make_shared<PotDecorator>(freshCactus);
        deserialized->deserialize(data);

        CHECK_EQ(deserialized->typeName(), "PotDecorator");
    }
}

TEST_CASE("RibbonDecorator modifies name, price, and typeName") {
    auto rose = std::make_shared<Rose>();
    auto ribboned = std::make_shared<RibbonDecorator>(rose);

    CHECK_EQ(ribboned->getName(), "Rose with Ribbon");
    CHECK_GT(ribboned->getPrice(), rose->getPrice());
    CHECK_EQ(ribboned->typeName(), "RibbonDecorator");

    SUBCASE("blueprintClone creates a new decorated plant") {
        auto clone = ribboned->blueprintClone();
        CHECK(clone.get() != nullptr);
        CHECK_EQ(clone->typeName(), "RibbonDecorator");
    }

    SUBCASE("serialize adds prefix and deserialize restores state") {
        auto data = ribboned->serialize();
        CHECK(data.find("Ribbon|") == 0);

        auto freshRose = std::make_shared<Rose>();
        auto deserialized = std::make_shared<RibbonDecorator>(freshRose);
        deserialized->deserialize(data);

        CHECK_EQ(deserialized->typeName(), "RibbonDecorator");
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

    SUBCASE("blueprintClone preserves decorator chain") {
        auto clone = decorated->blueprintClone();
        CHECK(clone.get() != nullptr);
        CHECK_GT(clone->getPrice(), rose->getPrice());
    }

    SUBCASE("serialize stores nested structure") {
        auto serialized = decorated->serialize();
        CHECK(serialized.find("Ribbon|") == 0);
        CHECK(serialized.find("Pot|") != std::string::npos);
        CHECK(serialized.find("GiftWrap|") != std::string::npos);
    }
}