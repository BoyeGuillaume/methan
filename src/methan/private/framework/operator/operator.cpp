#include <methan/private/framework/operator/operator.hpp>
#include <methan/private/framework/tensor/tensor_block.hpp>
#include <methan/private/framework/tensor/tensor.hpp>
#include <methan/private/framework/operator/operator_factory.hpp>


Methan::AbstractOperator::AbstractOperator(AbstractOperatorFactory* factory, Uuid uuid, const std::vector<TensorBlock*>& blocks)
: AbstractTask(factory->context(), uuid, factory->name()),
  m_factory(factory),
  m_blocks(std::move(blocks))
{
#ifdef METHAN_EXPAND_ASSERTION
    for(size_t i = 0; i < m_blocks.size(); ++i) {
        METHAN_ASSERT_NON_NULL(m_blocks[i]);
    }
#endif
}

METHAN_API Methan::AbstractOperator::~AbstractOperator()
{

}


