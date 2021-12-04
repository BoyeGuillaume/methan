#include <catch2/catch_test_macros.hpp>
#include <methan/private/framework/framework.hpp>
#include <methan/private/framework/memory/dataflow.hpp>
#include <methan/private/framework/memory/allocator.hpp>
#include <methan/private/framework/memory/memory.hpp>
#include <methan/private/framework/device.hpp>
#include <methan/private/private_context.hpp>
#include <methan/private/framework/threading/scheduler.hpp>
#include <methan/private/framework/threading/task.hpp>
#include <methan/core/context.hpp>
#include <thread>
#include <cstring>

using namespace Methan;

TEST_CASE("Allocation are working correctly on the heap", "[allocation]")
{
    Context context = ContextBuilder()
        .add_logger_stdout(ELogLevel::Debug)
        .register_cpu_as_candidate(2) // By default github action only gave us 2 cpu :(
        .set_heap_memory_limits(5_MB)
        .build();

    REQUIRE(context->devices.size() == 1);
    REQUIRE(context->devices[0]->descriptor().core == 2); // Notice that if you run this test on a computer with less than 2 it will fail
                                                          // But again : who does that nowadays ???
    REQUIRE(context->devices[0]->descriptor().deviceType == EDeviceType::Cpu);
    REQUIRE(context->memories.size() == 1);
    REQUIRE(context->memories[0]->descriptor().maxUsage == 5_MB);
    REQUIRE(context->memories[0]->descriptor().memoryType == EMemoryType::CpuHeap);
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

#ifdef METHAN_EXPAND_ASSERTION
    REQUIRE_THROWS_AS([&]() {
        context->memories[0]->allocator()->free(block2);
    }(), Methan::Exception);
#endif

    delete block2;
    REQUIRE(context->memories[0]->allocator()->current_usage() == 0);
    REQUIRE(context->memories[0]->allocator()->cumulated_usage() == 5_MB);

    Methan::free(context);
}

TEST_CASE("Heap flows are correctly working", "[flows]")
{
    Context context = ContextBuilder()
        .add_logger_stdout(ELogLevel::Debug)
        .register_cpu_as_candidate(2)
        .set_heap_memory_limits(10_MB)
        .build();

    Uuid heapUuid = context->memories[0]->uuid();
    DataBlock* b1 = context->memories[0]->allocator()->alloc(3_MB);
    DataBlock* b2 = context->memories[0]->allocator()->alloc(3_MB);

    REQUIRE(b1->concurrent_read() == 0);
    REQUIRE(b1->concurent_write() == 0);
    REQUIRE(b2->concurrent_read() == 0);
    REQUIRE(b2->concurent_write() == 0);

    const char* str = "Did I ever tell you what the definition of insanity is? Insanity is doing the exact "
                      "same fucking thing over and over again expecting shit to change.\0";
    memcpy(std::get<void*>(b1->handle()), str, strlen(str) + 1);

    // Copy b1 -> b2 on (strlen(str) / 15) with element size of 15
    AbstractDataFlowFactory* flowsFactory = context->flowsFactories[UuidPair(heapUuid, heapUuid)];
    AbstractDataFlow* flow = flowsFactory->initiate_flow(b1,
        b2,
        { FlowPosition(strlen(str) + 1, 0_B) },
        { FlowPosition(strlen(str) + 1, 2_MB) },
        generate_uuid(context));

    REQUIRE(flow->source() == b1);
    REQUIRE(flow->destination() == b2);
    REQUIRE(!flow->running());
    REQUIRE(!flow->terminated());

    REQUIRE_NOTHROW(flow->start());

    REQUIRE(!flow->running());
    REQUIRE(flow->terminated());
    REQUIRE(!flow->flashingError());
    REQUIRE(flow->successfull());
    REQUIRE(memcmp((uint8_t*) std::get<void*>(b2->handle()) + 2_MB, str, strlen(str)) == 0);

    REQUIRE(b1->concurrent_read() == 0);
    REQUIRE(b1->concurent_write() == 0);
    REQUIRE(b2->concurrent_read() == 0);
    REQUIRE(b2->concurent_write() == 0);
    delete flow;


    std::array<uint32_t, 16> random_uint32_t = {
        0x90870e98, 0x4932b3d0, 0x2de6341f, 0x057479ba,
        0xc2b66576, 0x4048a2c5, 0x3cfe22a6, 0x059600c5,
        0x4f62a8a3, 0xaba316a7, 0x713e40d3, 0xd3b7629d,
        0x215b337d, 0xd6886f08, 0xb03988fa, 0xb601540f
    };
    
#ifdef METHAN_EXPAND_ASSERTION
    REQUIRE_THROWS_AS([&](){
        auto* flow = flowsFactory->initiate_flow(b1,
                                                 b2,
        { FlowPosition(1_MB + 1, 0_B) },
        { FlowPosition(1_MB + 1, 2_MB) },
        generate_uuid(context));
    }(), Methan::Exception);

    REQUIRE_THROWS_AS([&](){
        auto* flow = flowsFactory->initiate_flow(b1,
                                                 b2,
        { FlowPosition(5_MB, 0_B) },
        { FlowPosition(5, 2_MB) },
        generate_uuid(context));
    }(), Methan::Exception);
#endif

    memcpy(std::get<void*>(b1->handle()), random_uint32_t.data(), random_uint32_t.size() * sizeof(uint32_t));
    flow = flowsFactory->initiate_flow(b1,
        b2,
        { FlowPosition(random_uint32_t.size(), 0_B, sizeof(uint32_t)) },
        { FlowPosition(random_uint32_t.size(), 2_MB, sizeof(uint32_t), 5 * sizeof(uint32_t)) },
        generate_uuid(context));
    
    REQUIRE_NOTHROW(flow->start());
    delete flow;

    for(size_t i = 0; i < random_uint32_t.size(); ++i)
    {
        size_t offset = 2_MB + 5 * i * sizeof(uint32_t);
        uint8_t* data = ((uint8_t*) std::get<void*>(b2->handle())) + offset;
        REQUIRE(*reinterpret_cast<uint32_t*>(data) == random_uint32_t[i]);
    }
    
    delete b1;
    delete b2;
    Methan::free(context);
}

TEST_CASE("Detect memory leak (visual test)", "[alloc]")
{
    Context context = ContextBuilder()
        .add_logger_stdout(ELogLevel::Debug)
        .register_cpu_as_candidate(2) // By default github action only gave us 2 cpu :(
        .set_heap_memory_limits(5_MB)
        .build();

    DataBlock* b1 = context->memories[0]->allocator()->alloc(3_MB);        

    Methan::free(context);
}

TEST_CASE("Base Async heap copy", "[async flow]")
{
    Context context = ContextBuilder()
        .add_logger_stdout(ELogLevel::Debug)
        .register_cpu_as_candidate(2) // By default github action only gave us 2 cpu to work with :(
        .set_heap_memory_limits(3_MB)
        .build();

    AbstractAllocator* heapAllocator = context->memories[0]->allocator();
    AbstractDataFlowFactory* heapFlowFactory = context->flowsFactories[std::make_pair(heapAllocator->uuid(), heapAllocator->uuid())];

    DataBlock* b1 = heapAllocator->alloc(1_MB);
    DataBlock* b2 = heapAllocator->alloc(1_MB);
    
    const char* str = "Did I ever tell you what the definition of insanity is? Insanity is doing the exact "
                      "same fucking thing over and over again expecting shit to change.\0";
    memcpy(std::get<void*>(b1->handle()), str, strlen(str) + 1);

    
    AbstractDataFlow* flow = heapFlowFactory->initiate_flow(b1,
                                                            b2,
                                                            { FlowPosition(1_MB) },
                                                            { FlowPosition(1_MB) },
                                                            generate_uuid(context));
    std::thread worker(&AbstractDataFlow::start, flow);

    flow->signal()->wait([](uint32_t value) {
        return value & (uint32_t) EDataFlowStateFlag::Done;
    }); // Wait for the copy to terminate

    REQUIRE(memcmp(std::get<void*>(b2->handle()), str, strlen(str) + 1) == 0);
    REQUIRE(!flow->running());
    REQUIRE(flow->terminated());
    REQUIRE(flow->successfull());
    REQUIRE(!flow->flashingError());


    // Cleanup duty
    worker.join();
    delete flow;
    delete b1;
    delete b2;

    Methan::free(context);
}