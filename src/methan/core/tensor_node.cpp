#include <methan/core/tensor_node.hpp>
#include <methan/private/framework/computational/tensor_node.hpp>

METHAN_API Methan::TensorNode::TensorNode(TensorNodeImplPtr self)
: m_self(self)
{}

