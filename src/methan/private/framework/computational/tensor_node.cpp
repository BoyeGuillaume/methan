#include <methan/private/framework/computational/tensor_node.hpp>
#include <methan/private/framework/operator/operator_factory.hpp>

METHAN_API Methan::__private__::TensorNodeImpl::TensorNodeImpl(StringIdentifier identifier, const std::vector<TensorNodeImplPtr>& dependencies, AbstractOperatorFactory* operator_factory)
: Contextuable(operator_factory),
  m_dependencies(dependencies),
  m_operator_factory(operator_factory),
  m_id(identifier)
{}

