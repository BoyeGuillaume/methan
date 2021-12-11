#include <methan/core/tensor_node.hpp>
#include <methan/private/tensor_node_impl.hpp>

METHAN_API Methan::TensorNode::TensorNode(TensorNodeImplPtr self)
: m_self(self)
{}

METHAN_API Methan::__private__::TensorNodeImpl::TensorNodeImpl(const std::vector<TensorNodeImplPtr>& dependencies,
    StringIdentifier op_id)
: m_dependencies(dependencies),
  m_op_id(op_id)
{}