#include <methan/private/framework/operator/operator_factory.hpp>
#include <methan/private/private_context.hpp>
#include <methan/private/private_formatter.hpp>
#include <methan/utility/assertion.hpp>
#include <methan/private/framework/tensor/tensor_block.hpp>

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

METHAN_API Methan::AbstractOperator* Methan::AbstractOperatorFactory::create_operator(const Uuid& uuid, const std::vector<TensorBlock*>& inputs, const std::vector<TensorBlock*>& outputs, const std::vector<Parameter>& parameters)
{
#ifdef METHAN_EXPAND_ASSERTION
    std::vector<SlicedTensorShape> inputs_;
    std::vector<SlicedTensorShape> outputs_;
    inputs_.reserve(inputs.size());
    outputs_.reserve(outputs.size());

    for(size_t i = 0; i < inputs.size(); ++i)
    {
        METHAN_ASSERT_NON_NULL(inputs[i]);
        inputs_.push_back(inputs[i]->shape());
    }

    for(size_t i = 0; i < outputs.size(); ++i)
    {
        METHAN_ASSERT_NON_NULL(outputs[i]);
        outputs_.push_back(outputs[i]->shape());
    }

    if(!is_valid(inputs_, outputs_, parameters))   
    {
        METHAN_LOG_ERROR(context()->logger, "OperatorFactory(\"{}\")::create_operator() failed as the ::is_valid(inputs) return false", std::to_string(m_identifier));
        METHAN_THROW_EXCEPTION("Invalid argument exception, cannot call this operator with this arguments", ExceptionType::IllegalArgument);
    }
#endif

    AbstractOperator* ops = __create_operator(uuid, inputs, outputs, parameters);
    METHAN_ASSERT_NON_NULL(ops); 

    return ops;
}
