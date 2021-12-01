#include <catch2/catch_test_macros.hpp>
#include <methan/private/framework/tensor/tensor.hpp>
#include <methan/private/framework/tensor/tensor_block.hpp>
#include <methan/private/framework/memory/dataflow.hpp>
#include <methan/private/private_context.hpp>
#include <methan/core/context.hpp>

using namespace Methan;

TEST_CASE("Tensor work correctly", "[tensor]")
{
    Context context = ContextBuilder()
        .add_logger_stdout(ELogLevel::Debug)
        .register_cpu_as_candidate(2)
        .build();
    
    // Create a tensor
    TensorShape shape = { 9, 9 };
    Tensor *tensor = new Tensor(context, generate_uuid(context), shape, Methan::DType::Int32);

    // Testing the tensor
    REQUIRE(tensor->memsize() == 81 * sizeof(uint32_t));
    REQUIRE(tensor->size() == 81);
    REQUIRE(tensor->dtype() == Methan::DType::Int32);

    // Allocate memory in heap and use this memory to create a slice
    SlicedTensorShape sliced(shape, { 2, 2 }, { 4, 4 });
    TensorBlockDescriptor descriptor(tensor->uuid(), generate_uuid(context), sliced);

    DataBlock* block = context->memories[0]->allocator()->alloc(sliced.size() * size_of(tensor->dtype()));
    REQUIRE_THROWS_AS([&](){
        block->acquire_safe_read_access();
        TensorBlock block(tensor, block, descriptor);
    }(), Methan::Exception);

    block->release_read_access();
    TensorBlock* tensorBlock = new TensorBlock(tensor, block, descriptor);
    REQUIRE(context->owned_blocks.size() == 1);
    REQUIRE(tensorBlock->descriptor().tensor == tensor->uuid());
    REQUIRE(tensorBlock->state() == ETensorBlockState::Future);
    
#ifdef METHAN_EXPAND_ASSERTION
    REQUIRE_THROWS_AS(tensorBlock->acquire_read_access(), Methan::Exception);
#endif

    uint32_t* ptr = (uint32_t*) std::get<void*>(tensorBlock->acquire_write_access());
    for(uint32_t m = 0; m < 4; ++m) {
        for(uint32_t n = 0; n < 4; ++n) {
            *(ptr + tensorBlock->descriptor().sliced.offset_of({ m, n })) = 1;
        }
    }
    tensorBlock->release_write_access();
    delete tensorBlock;

    REQUIRE(block->is_locked() == false);
    REQUIRE(block->is_weak() == false);
    REQUIRE(context->owned_blocks.size() == 0);

    context->memories[0]->allocator()->free(block);
    REQUIRE(block->is_weak());

    delete block;
    delete tensor;
    Methan::free(context);
}

