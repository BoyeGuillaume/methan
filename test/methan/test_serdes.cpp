#include <string>
#include <sstream>
#include <memory>

#include <catch2/catch_test_macros.hpp>
#include <methan/core/except.hpp>
#include <methan/core/serializable.hpp>

struct TestSerializableClass
{
    uint32_t d1;
    std::string str;
    std::vector<float> floats;
    std::array<uint8_t, 16> values;
    std::vector<int> empty;

    METHAN_SERDE_GENERATOR(TestSerializableClass, d1, str, floats, values, empty)
};

TEST_CASE("Serialize and deserialize complex object", "[serialize]")
{
    std::stringstream ss;
    {
        TestSerializableClass k;
        k.d1 = 0xACDC;
        k.str = "this is a fk** smoking pipe";
        k.floats = { 6.564f, 3.51f, 2.0f };
        memcpy(k.values.data(), "5165d1czzefzefe", 16);
        Methan::Serde::serialize(ss, k);
    }

    {
        TestSerializableClass k = Methan::Serde::deserialize<TestSerializableClass>(ss);
        REQUIRE(k.d1 == 0xACDC);
        REQUIRE(k.str == "this is a fk** smoking pipe");
        REQUIRE(k.floats.size() == 3);
        REQUIRE(k.floats[0] == 6.564f);
        REQUIRE(k.floats[1] == 3.51f);
        REQUIRE(k.floats[2] == 2.0f);
        REQUIRE(memcmp((char*) k.values.data(), "5165d1czzefzefe", 16) == 0);
        REQUIRE(k.empty.empty());
    }
}

TEST_CASE("Serialize field correctly", "[serialize]")
{
    {
        std::stringstream ss;
        Methan::Serde::serialize<uint32_t>(ss, 50);
        REQUIRE(Methan::Serde::deserialize<uint32_t>(ss) == 50);
    }

    {
        std::stringstream ss;
        Methan::Serde::serialize<float>(ss, 0.5165f);
        REQUIRE(Methan::Serde::deserialize<float>(ss) == 0.5165f);
    }

    {
        std::stringstream ss;
        Methan::Serde::serialize<std::string>(ss, "hello world");
        REQUIRE(Methan::Serde::deserialize<std::string>(ss) == "hello world");
    }

    {
        std::stringstream ss;
        std::vector<uint32_t> test = { 8, 3, 2 };
        Methan::Serde::serialize(ss, test);
        test.clear();
        Methan::Serde::deserialize(ss, test);
        REQUIRE(test.size() == 3);
        REQUIRE(test[0] == 8);
        REQUIRE(test[1] == 3);
        REQUIRE(test[2] == 2);
    }
}

