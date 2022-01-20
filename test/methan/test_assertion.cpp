#include <catch2/catch_test_macros.hpp>

#include <methan/core/configuration.hpp>
#ifndef METHAN_DEBUG
#define METHAN_DEBUG
#endif
#include <methan/utility/assertion.hpp>

TEST_CASE("Exception thrown correctly", "[macro]")
{
    REQUIRE_THROWS_AS([]() {
        METHAN_THROW_EXCEPTION("A random exception always find the way to your heart", Methan::ExceptionType::Unknown);
    }(), Methan::Exception);

    try {
        METHAN_THROW_EXCEPTION("Hello, World", Methan::ExceptionType::IllegalArgument);
    }
    catch (Methan::Exception& e) {
        REQUIRE(strcmp(e.file(), __FILE__) == 0);
        REQUIRE(e.line() == (__LINE__ - 4));
        REQUIRE(e.type() == Methan::ExceptionType::IllegalArgument);
    }
}

TEST_CASE("Testing the assertion", "[macro]")
{
    CHECK_THROWS_AS([]() { METHAN_FORCE_ASSERT(false, Methan::ExceptionType::IllegalArgument, "An exception occured"); }(), Methan::Exception);
    CHECK_THROWS_AS([]() { METHAN_FORCE_ASSERT_ARGUMENT(true == false); }(), Methan::Exception);
    CHECK_THROWS_AS([]() { METHAN_FORCE_ASSERT_INDEX(115, 115); }(), Methan::Exception);
    CHECK_THROWS_AS([]() { METHAN_FORCE_ASSERT_NON_NULL(nullptr); }(), Methan::Exception);
    CHECK_THROWS_AS([]() { METHAN_FORCE_ASSERT_NULL((void*) 0x5); }(), Methan::Exception);

    CHECK_NOTHROW([]() { METHAN_FORCE_ASSERT(true, Methan::ExceptionType::Unknown, "Happy when no exception"); }());
    CHECK_NOTHROW([]() { METHAN_FORCE_ASSERT_ARGUMENT(1 == 1); }());
    CHECK_NOTHROW([]() { METHAN_FORCE_ASSERT_INDEX(5, 6); }());
    CHECK_NOTHROW([]() { METHAN_FORCE_ASSERT_NON_NULL((void*) 0x05); }());
    CHECK_NOTHROW([]() { METHAN_FORCE_ASSERT_NULL(nullptr); }());
}
