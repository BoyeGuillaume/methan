#include <catch2/catch_test_macros.hpp>
#include <methan/private/framework/framework.hpp>
#include <methan/private/framework/memory/allocator.hpp>
#include <methan/private/framework/memory/memory.hpp>
#include <methan/private/framework/device.hpp>
#include <methan/private/private_context.hpp>
#include <methan/core/context.hpp>
#include <thread>

using namespace Methan;

TEST_CASE("Allocation are working correctly on the heap", "[allocation]") {
    Context context = ContextBuilder()
        .add_logger_stdout(ELogLevel::Debug)
        .register_cpu_as_candidate(4)
        .set_heap_memory_limits(5_MB)
        .build();

    REQUIRE(context->devices.size() == 1);
    REQUIRE(context->devices[0]->descriptor().core == 4);
    REQUIRE(context->devices[0]->descriptor().deviceType == EDeviceType::Cpu);
    REQUIRE(context->memories.size() == 1);
    REQUIRE(context->memories[0]->descriptor().maxUsage == 5_MB);
    REQUIRE(context->memories[0]->descriptor().memoryType == EMemoryType::CpuHeap);
    REQUIRE(context->memories[0]->descriptor().capabilitiesFlag & EMemoryCapabilitiesFlag::SupportKeepAllocationView);
    REQUIRE(context->memories[0]->descriptor().alignement == 0);
    REQUIRE(context->memories[0]->allocator()->current_usage() == 0);
    REQUIRE(context->memories[0]->allocator()->cumulated_usage() == 0);

    DataBlock *block = context->memories[0]->allocator()->alloc(3_MB);
    REQUIRE(!block->is_weak());
    REQUIRE(block->size() == 3_MB);
    REQUIRE(block->reference_allocator() == context->memories[0]->allocator());
    REQUIRE(block->uuid() != Methan::Uuid{nullptr});
    REQUIRE_NOTHROW(std::get<void*>(block->handle()));
    memset(std::get<void*>(block->handle()), 0x5, (size_t) 3_MB); // Except no error
    memset(std::get<void*>(block->handle()), 0x0, (size_t) 2_MB);

    REQUIRE_THROWS_AS([context](){
        DataBlock* block = context->memories[0]->allocator()->alloc(3_MB);
    }(), Methan::Exception);

    DataBlock *block2 = context->memories[0]->allocator()->alloc(2_MB);
    REQUIRE(!block2->is_weak());
    memcpy(std::get<void*>(block->handle()), std::get<void*>(block2->handle()), (size_t) block2->size());
    REQUIRE(context->memories[0]->allocator()->current_usage() == 5_MB);
    REQUIRE(context->memories[0]->allocator()->cumulated_usage() == 5_MB);

    delete block;
    REQUIRE(context->memories[0]->allocator()->current_usage() == 2_MB);
    context->memories[0]->allocator()->free(block2);
    REQUIRE(block2->is_weak());

    REQUIRE_THROWS_AS([&]() {
        context->memories[0]->allocator()->free(block2);
    }(), Methan::Exception);

    delete block2;
    REQUIRE(context->memories[0]->allocator()->current_usage() == 0);
    REQUIRE(context->memories[0]->allocator()->cumulated_usage() == 5_MB);

    free(context);
}
