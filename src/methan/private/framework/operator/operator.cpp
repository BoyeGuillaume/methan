#include <methan/private/framework/operator/operator.hpp>
#include <methan/private/framework/tensor/tensor_block.hpp>
#include <methan/private/framework/tensor/tensor.hpp>
#include <methan/private/framework/operator/operator_factory.hpp>


Methan::AbstractOperator::AbstractOperator(AbstractOperatorFactory* factory, Uuid uuid, const std::vector<TensorBlock*>& inputs, const std::vector<TensorBlock*>& outputs)
: AbstractTask(factory->context(), uuid, factory->name()),
  m_factory(factory),
  m_inputs(inputs),
  m_outputs(outputs)
{
#ifdef METHAN_EXPAND_ASSERTION
    for(size_t i = 0; i < m_inputs.size(); ++i) {
        METHAN_ASSERT_NON_NULL(m_inputs[i]);
    }

    for(size_t i = 0; i < m_outputs.size(); ++i) {
        METHAN_ASSERT_NON_NULL(m_outputs[i]);
    }
#endif
}

METHAN_API Methan::AbstractOperator::~AbstractOperator()
{

}


