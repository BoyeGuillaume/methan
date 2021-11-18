#include <catch2/catch_test_macros.hpp>
#include <methan/core/lib.hpp>

TEST_CASE("Verifying that true=true", "[assertions]") {
    REQUIRE(true == true);
}

TEST_CASE("Verifying that returnTrue return true", "[assertions]") {
    REQUIRE(returnTrue() == true);
}
