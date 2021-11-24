#pragma once

#include <catch2/catch_test_macros.hpp>
#include <methan/private/framework/cpu.hpp>
#include <methan/private/private_context.hpp>
#include <methan/core/context.hpp>

TEST_CASE("CPU correctly registered", "[cpu]")
{
    Methan::Context context = Methan::ContextBuilder()
        .add_logger_stdout(Methan::ELogLevel::Debug)
        .build();

    Methan::DeviceDescriptor* descriptor = Methan::cpu_descriptor(context);
    REQUIRE(descriptor->deviceType == Methan::EDeviceType::Cpu);
    delete descriptor;

    Methan::free(context);
}
