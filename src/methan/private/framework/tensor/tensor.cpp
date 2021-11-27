#include <methan/private/framework/tensor/tensor.hpp>
#include <methan/private/private_context.hpp>
#include <methan/private/private_formatter.hpp>

METHAN_API Methan::Tensor::Tensor(Context context, const Uuid& uuid, const TensorShape& shape, DType dtype)
: Contextuable(context),
  m_uuid(uuid),
  m_shape(shape),
  m_dtype(dtype)
{
    METHAN_LOG_INFO(context->logger, "New Tensor({}) registered with shape {}", m_uuid, m_shape);
}

METHAN_API Methan::Tensor::~Tensor()
{
    METHAN_LOG_INFO(context()->logger, "Tensor({}) destructed", m_uuid);
}
