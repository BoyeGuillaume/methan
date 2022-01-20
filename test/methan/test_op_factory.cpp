#include <catch2/catch_test_macros.hpp>
#include <methan/private/framework/operator/operator_factory.hpp>
#include <methan/private/framework/operator/operator_registery.hpp>
#include <methan/private/private_context.hpp>
#include <methan/core/context.hpp>

using namespace Methan;

static OpDependencyDescriptor* __descriptor;

class OpTestFactory : public AbstractOperatorFactory
{
public:
    OpTestFactory(Context context) : AbstractOperatorFactory(context, METHAN_IDENTIFIER("OpTestFactory"), EOpFactoryFlag::KernelSupportCpu, 1, 0x1)
    { }

    bool __create_new_op_dependencies(OpDependencyDescriptor* allocated_memory, const std::vector<size_t>& input_ranks, const std::vector<size_t>& output_ranks, const std::vector<Parameter>& parameters)
    {
        new (allocated_memory) OpDependencyDescriptor(*__descriptor);
        return false;
    }

    virtual std::optional<std::vector<TensorShape>> inferred_result_shape(const std::vector<TensorShape*>& inputs, const std::vector<Parameter>& parameters) const override
    {
        return std::optional<std::vector<TensorShape>>();
    }

protected:
    AbstractOperator* __create_operator(const std::vector<TensorBlock*>& inputs, const std::vector<TensorBlock*>& outputs, const std::vector<Parameter>& parameters, const OpCreateDescriptor& descriptor) override
    {
        return nullptr;
    }
};

METHAN_REGISTER_OP_FACTORY(OpTestFactory);


TEST_CASE("Assert that the registration process work correctly", "[op_factory]")
{
    Context context = ContextBuilder()
        .add_logger_stdout(ELogLevel::Debug)
        .build();
    
    AbstractOperatorFactory* factory = context->registry->find(METHAN_IDENTIFIER("OpTestFactory"));
    REQUIRE(factory != nullptr);

    Methan::free(context);
}

TEST_CASE("Test the descriptor validity check", "[op_factory]")
{
    Context context = ContextBuilder()
        .add_logger_stdout(ELogLevel::Debug)
        .build();
    OpTestFactory* factory = new OpTestFactory(context);

    OpDependencyDescriptor descriptor(1, 1);
    __descriptor = &descriptor;

    descriptor(0, 0) = {
        OpDependencyCoordinateDescriptor{ 1, 0, EOpDependency::Everything },
    };

    TensorShape a = { 5, 5 };
    TensorShape b = { 5, 5 };

    REQUIRE(factory->is_valid({SlicedTensorShape(a, { 0, 0 }, { 5, 5 })}, {SlicedTensorShape(b, { 2, 2 }, { 1, 1 })}, { 0 }) == true);
    REQUIRE(factory->is_valid({SlicedTensorShape(a, { 1, 0 }, { 4, 5 })}, {SlicedTensorShape(b, { 2, 2 }, { 1, 1 })}, { 0 }) == true);
    REQUIRE(factory->is_valid({SlicedTensorShape(a, { 0, 0 }, { 4, 4 })}, {SlicedTensorShape(b, { 2, 2 }, { 1, 1 })}, { 0 }) == false);
    REQUIRE(factory->is_valid({SlicedTensorShape(a, { 0, 1 }, { 5, 4 })}, {SlicedTensorShape(b, { 2, 2 }, { 1, 1 })}, { 0 }) == false);

    descriptor(0, 0) = {
        OpDependencyCoordinateDescriptor{ 0, 1, EOpDependency::ComponentWise },
    };

    REQUIRE(factory->is_valid({
        SlicedTensorShape(TensorShape({ 2, 10 }), { 0, 0 }, { 2, 10 })
    }, {
        SlicedTensorShape(TensorShape({ 1, 2 }), { 0, 0 }, { 1, 2 })
    }, { 1 }));

    REQUIRE(factory->is_valid({
        SlicedTensorShape(TensorShape({ 2, 10 }), { 0, 0 }, { 2, 10 })
    }, {
        SlicedTensorShape(TensorShape({ 1, 2 }), { 0, 1 }, { 1, 1 })
    }, { 1 }));

    REQUIRE(factory->is_valid({
        SlicedTensorShape(TensorShape({ 2, 10 }), { 0, 0 }, { 2, 10 })
    }, {
        SlicedTensorShape(TensorShape({ 1, 2 }), { 0, 0 }, { 1, 1 })
    }, { 1 }));

    REQUIRE(factory->is_valid({
        SlicedTensorShape(TensorShape({ 2, 10 }), { 0, 0 }, { 2, 10 })
    }, {
        SlicedTensorShape(TensorShape({ 1, 1 }), { 0, 0 }, { 1, 1 })
    }, { 1 }) == false);

    REQUIRE(factory->is_valid({
        SlicedTensorShape(TensorShape({ 6, 10 }), { 2, 0 }, { 3, 10 })
    }, {
        SlicedTensorShape(TensorShape({ 2, 6 }), { 0, 1 }, { 1, 4 })
    }, { 1 }) == false);

    REQUIRE(factory->is_valid({
        SlicedTensorShape(TensorShape({ 6, 10 }), { 2, 0 }, { 3, 10 })
    }, {
        SlicedTensorShape(TensorShape({ 2, 6 }), { 0, 1 }, { 1, 3 })
    }, { 1 }) == false);

    REQUIRE(factory->is_valid({
        SlicedTensorShape(TensorShape({ 6, 10 }), { 1, 0 }, { 3, 10 })
    }, {
        SlicedTensorShape(TensorShape({ 2, 6 }), { 0, 1 }, { 1, 3 })
    }, { 1 }));



    descriptor(0, 0) = {
        OpDependencyCoordinateDescriptor{ 0, 0, EOpDependency::kNearestNeighbor, OpDependencyNearestNeighborEXT{ 2, 0 } },
    };

    REQUIRE(factory->is_valid({
        SlicedTensorShape(TensorShape({ 6 }), { 0 }, { 6 })
    }, {
        SlicedTensorShape(TensorShape({ 2, 5 }), { 0, 0 }, { 2, 1 })
    }, { 2 }));

    REQUIRE(factory->is_valid({
        SlicedTensorShape(TensorShape({ 6 }), { 0 }, { 5 })
    }, {
        SlicedTensorShape(TensorShape({ 2 }), { 0 }, { 1 })
    }, { 2 }));

    REQUIRE(factory->is_valid({
        SlicedTensorShape(TensorShape({ 6 }), { 0 }, { 4 })
    }, {
        SlicedTensorShape(TensorShape({ 2 }), { 0 }, { 1 })
    }, { 2 }) == false);

    REQUIRE(factory->is_valid({
        SlicedTensorShape(TensorShape({ 6 }), { 1 }, { 5 })
    }, {
        SlicedTensorShape(TensorShape({ 2 }), { 0 }, { 1 })
    }, { 2 }) == false);

    REQUIRE(factory->is_valid({
        SlicedTensorShape(TensorShape({ 6 }), { 0 }, { 6 })
    }, {
        SlicedTensorShape(TensorShape({ 2 }), { 0 }, { 1 })
    }, { 2 }));

    REQUIRE(factory->is_valid({
        SlicedTensorShape(TensorShape({ 6 }), { 1 }, { 5 })
    }, {
        SlicedTensorShape(TensorShape({ 2 }), { 1 }, { 1 })
    }, { 2 }));



    descriptor(0, 0) = {
        OpDependencyCoordinateDescriptor{ 0, 0, EOpDependency::kNearestNeighbor, OpDependencyNearestNeighborEXT{ 2, 1 } },
    };

    REQUIRE(factory->is_valid({
        SlicedTensorShape(TensorShape({ 6 }), { 0 }, { 6 })
    }, {
        SlicedTensorShape(TensorShape({ 4, 5 }), { 0, 0 }, { 4, 1 })
    }, { 3 }));

    REQUIRE(factory->is_valid({
        SlicedTensorShape(TensorShape({ 6 }), { 3 }, { 3 })
    }, {
        SlicedTensorShape(TensorShape({ 4 }), { 3 }, { 1 })
    }, { 3 }));

    REQUIRE(factory->is_valid({
        SlicedTensorShape(TensorShape({ 6 }), { 4 }, { 2 })
    }, {
        SlicedTensorShape(TensorShape({ 4 }), { 3 }, { 1 })
    }, { 3 }) == false);

    // the above assertion only test that the op_descriptor is not reallocated at every single call to get_op_descriptor and is cached in a internal hash_map
    REQUIRE(factory->is_valid({SlicedTensorShape(a, { 1, 0 }, { 4, 5 })}, {SlicedTensorShape(b, { 2, 2 }, { 1, 1 })}, { 0 }) == true);
    REQUIRE(factory->is_valid({SlicedTensorShape(a, { 1, 0 }, { 4, 5 })}, {SlicedTensorShape(b, { 2, 2 }, { 1, 1 })}, { 3 }) == false);

    delete factory;
    Methan::free(context);
}

