#include <catch2/catch_test_macros.hpp>
#include <methan/utility/assertion.hpp>
#include <methan/private/computer.hpp>

#include <sstream>

using namespace Methan;

TEST_CASE("computer serialisation", "[serde]") {
    std::stringstream ss;
    
    {
        ComputerDescriptor computer;
        computer.identity.hostname = "localhost";
        computer.identity.port = 8080;
        computer.type = EComputerType::Observer;
        computer.uuid = Methan::Uuid({0x9915, 0x2134});
        Serde::serialize(ss, computer);
    }

    {
        ComputerDescriptor computer;
        Serde::deserialize(ss, computer);
        REQUIRE(computer.identity.hostname == "localhost");
        REQUIRE(computer.identity.port == 8080);
        REQUIRE(computer.type == EComputerType::Observer);
        REQUIRE(computer.uuid == Methan::Uuid({0x9915, 0x2134}));
    }
}
