#include <catch2/catch_test_macros.hpp>
#include <methan/private/framework/cpu.hpp>
#include <methan/private/private_context.hpp>
#include <methan/core/context.hpp>

TEST_CASE("CPU correctly registered", "[cpu]")
{
    Methan::Context context = Methan::ContextBuilder()
        .add_logger_stdout(Methan::ELogLevel::Debug)
        .register_cpu_as_candidate()
        .build();

    REQUIRE(context->devices.size() == 1);
    REQUIRE(context->devices[0]->descriptor().deviceType == Methan::EDeviceType::Cpu);

    Methan::free(context);
}
