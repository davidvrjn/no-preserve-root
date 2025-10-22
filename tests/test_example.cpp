#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include <vector>

TEST_CASE("vector push_back increases size") {
    std::vector<int> v;
    CHECK(v.empty());
    v.push_back(1);
    CHECK_EQ(v.size(), 1);
}

TEST_CASE("edge case: empty vector front access") {
    std::vector<int> v;
    CHECK(v.empty());
    // ensure no crash when checking size before access
    CHECK_EQ(v.size(), 0);
}
