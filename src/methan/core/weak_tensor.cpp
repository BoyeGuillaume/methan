#include <methan/core/weak_tensor.hpp>

METHAN_API Methan::WeakTensor::WeakTensor(StringIdentifier id, const TensorShape& shape, DType type)
: m_id(id), 
  m_shape(shape), 
  m_type(type)
{}


