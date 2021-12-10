#include <methan/private/framework/operator/operator_factory.hpp>
#include <methan/private/private_context.hpp>
#include <methan/private/private_formatter.hpp>
#include <methan/utility/assertion.hpp>

METHAN_API Methan::AbstractOperatorFactory::AbstractOperatorFactory(Context context, const StringIdentifier& identifier)
: Contextuable(context),
  m_identifier(identifier)
{
    m_name = "Operator{" + std::to_string(identifier) + "}";
}

METHAN_API Methan::AbstractOperatorFactory::~AbstractOperatorFactory()
{
    METHAN_LOG_DEBUG(context()->logger, "OperatorFactory(\"{}\") was destructed", std::to_string(m_identifier));
}

METHAN_API Methan::AbstractOperator* Methan::AbstractOperatorFactory::create_operator(const Uuid& uuid, const std::vector<TensorBlock*>& inputs, const std::vector<TensorBlock*>& outputs)
{
#ifdef METHAN_EXPAND_ASSERTION
    if(!is_valid(inputs, outputs))   
    {
        METHAN_LOG_ERROR(context()->logger, "OperatorFactory(\"{}\")::create_operator() failed as the ::is_valid(inputs) return false", std::to_string(m_identifier));
        METHAN_THROW_EXCEPTION("Invalid argument exception, cannot call this operator with this arguments", ExceptionType::IllegalArgument);
    }
#endif

    AbstractOperator* ops = __create_operator(uuid, inputs, outputs);
    METHAN_ASSERT_NON_NULL(ops); 

    return ops;
}

METHAN_API bool Methan::AbstractOperatorFactory::is_valid(const std::vector<TensorBlock*>& inputs, const std::vector<TensorBlock*>& outputs) const
{
#ifdef METHAN_EXPAND_ASSERTION
    for(size_t i = 0; i < inputs.size(); ++i) {
        METHAN_ASSERT_NON_NULL(inputs[i]);
    }
#endif

    return __is_valid(inputs);
}

