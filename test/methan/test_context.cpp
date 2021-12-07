#include <methan/core/context.hpp>
#include <catch2/catch_test_macros.hpp>
#include <methan/private/private_context.hpp>
#include <methan/private/framework/operator/operator_factory.hpp>
#include <methan/private/framework/operator/operator_registery.hpp>
#include <methan/private/framework/operator/operator.hpp>

using namespace Methan;

class TestingOperatorFactory : public AbstractOperatorFactory
{
public:
    TestingOperatorFactory(Context context)
    : AbstractOperatorFactory(context, METHAN_IDENTIFIER("TestingOp"))
    {

    }

    ~TestingOperatorFactory()
    {

    }

protected:
    AbstractOperator* __create_operator(const Uuid& uuid, const std::vector<TensorBlock*>& block) override
    {
        return nullptr;
    }

    bool __is_valid(const std::vector<TensorBlock*>& blocks) const
    {
        return blocks.size() == 2;   
    }
};

METHAN_REGISTER_OP_FACTORY(TestingOperatorFactory);


TEST_CASE("Construction of a context and free it (2x times) in sequential", "[context]")
{
    for(int i = 0; i < 2; ++i) {
        Methan::Context context = Methan::ContextBuilder()
            .add_logger_stdout(Methan::ELogLevel::Debug)
            .set_heap_memory_limits(5_MB)
            .build();
        Methan::free(context);
    }
}

TEST_CASE("Construction of two context in parrallel", "[context]")
{
    Methan::Context context1 = Methan::ContextBuilder()
        .add_logger_stdout(Methan::ELogLevel::Debug)
        .build();
    Methan::Context context2 = Methan::ContextBuilder()
        .add_logger_stdout(Methan::ELogLevel::Debug)
        .build();
    Methan::free(context2);
    Methan::free(context1);
}


TEST_CASE("Test that operators are correctly registered during context creation", "[context]")
{
    Context context = Methan::ContextBuilder()
        .add_logger_stdout(ELogLevel::Debug)
        .register_cpu_as_candidate(2)
        .set_heap_memory_limits(5_MB)
        .build();

    REQUIRE(context->registry != nullptr);
    REQUIRE(context->registry->find(METHAN_IDENTIFIER("TestingOp")) != nullptr);
    REQUIRE(context->registry->find(METHAN_IDENTIFIER("ThisIsDefinitlyNotAnExistingOp")) == nullptr);

    Methan::free(context);
}
