#include <methan/core/context.hpp>
#include <catch2/catch_test_macros.hpp>
#include <methan/private/net/socket.hpp>
#include <methan/private/private_context.hpp>
#include <methan/core/context.hpp>

TEST_CASE("QueryDNS work as expected", "[net]") {
    Methan::Context context = Methan::ContextBuilder()
        .add_logger_stdout(Methan::ELogLevel::Debug)
        .build();
    Methan::init_network(context);

    tl::expected<Methan::ResolvedHost, Methan::EDNSQueryErrorType> output = Methan::queryDNS(context, "www.google.fr", Methan::IpType::Ipv4);
    REQUIRE(output.has_value());

    // If that website exists is just to make me mad...
    output = Methan::queryDNS(context, "www.aeffezaf.azrgeth.com", Methan::IpType::Ipv4);
    REQUIRE(!output.has_value());
    REQUIRE(output.error() == Methan::EDNSQueryErrorType::HostNotFound);

    Methan::free(context);
}

