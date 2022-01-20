#include <unordered_set>
#include <sstream>

#include <catch2/catch_test_macros.hpp>
#include <methan/utility/uuid.hpp>
#include <methan/private/private_formatter.hpp>
#include <methan/private/private_context.hpp>
#include <methan/core/serializable.hpp>

TEST_CASE("UUID correctly compare from one another", "[uuid]")
{
    Methan::Uuid a(0x56FF5168, 0xFF326518);
    Methan::Uuid b(0x56FF5168, 0xFF000000);
    Methan::Uuid c(0x56000000, 0xFF3265FF);

    REQUIRE(a == a);
    REQUIRE(a != b);
    REQUIRE(b != c);
    REQUIRE(a > b);
    REQUIRE(a > c);
    REQUIRE(a >= b);
    REQUIRE(a >= a);
    REQUIRE(c < b);
    REQUIRE(c <= c);
    REQUIRE(b == b);
    REQUIRE(b <= a);
    REQUIRE(c <= a);
    REQUIRE(c < a);
}

TEST_CASE("UUID correctly translated to string", "[uuid]")
{
    Methan::Uuid nulluuid{nullptr};
    REQUIRE(nulluuid.to_string() == "00000000-0000-0000-0000-000000000000");
    REQUIRE(Methan::Uuid{0x123e4567e89b12d3ULL, 0xa456426614174000ULL}.to_string() == "123e4567-e89b-12d3-a456-426614174000");
}

TEST_CASE("Random UUID generated (Collision test) [heavy]", "[uuid]")
{
    Methan::UuidFactory factory;
    std::unordered_set<Methan::Uuid> generated;

    for(size_t i = 0; i < 500000; ++i)
    {
        Methan::Uuid uuid = factory();
        REQUIRE(generated.count(uuid) == 0);
        generated.insert(uuid);
    }
}

TEST_CASE("spdlog accept uuid", "[uuid]")
{
    Methan::Uuid uuid = {0x123e4567e89b12d3ULL, 0xa456426614174000ULL};
    Methan::Context context = Methan::ContextBuilder()
        .add_logger_stdout(Methan::ELogLevel::Debug)
        .build();

    METHAN_LOG_DEBUG(context->logger, "This is the generated uuid : {}", uuid);

    Methan::free(context);
}

TEST_CASE("uuid can be serialize", "[uuid]")
{
    Methan::UuidFactory factory;
    for(size_t i = 0; i < 10; ++i)
    {
        std::stringstream ss;
        Methan::Uuid uuid = factory();
        Methan::Serde::serialize(ss, uuid);
        REQUIRE(uuid == Methan::Serde::deserialize<Methan::Uuid>(ss));
    }
}

