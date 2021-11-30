#include <methan/private/framework/tensor/tensor_block.hpp>
#include <methan/private/private_context.hpp>
#include <methan/private/private_formatter.hpp>

METHAN_API Methan::TensorBlock::TensorBlock(Tensor* tensor, DataBlock* dataBlock, const TensorBlockDescriptor& descriptor)
: Contextuable(tensor),
  m_read_access(0),
  m_write_access(0),
  m_state(ETensorBlockState::Future),
  m_descriptor(descriptor),
  m_tensor(tensor),
  m_data_block(dataBlock)
{
    METHAN_ASSERT_ARGUMENT(dataBlock->context() == tensor->context());
    METHAN_ASSERT_ARGUMENT(dataBlock->size() > size_of(tensor->dtype()) * descriptor.sliced.size());
    METHAN_ASSERT_NON_NULL(m_data_block);
    m_data_block->acquire_safe_write_access();
}

METHAN_API Methan::TensorBlock::~TensorBlock()
{
    m_data_block->release_write_access();
}

METHAN_API Methan::DataBlock::PtrType Methan::TensorBlock::acquire_read_access()
{
    std::lock_guard guard(m_mutex);
#ifdef METHAN_EXPAND_ASSERTION
    if(m_write_access > 0)
    {
        METHAN_LOG_ERROR(context()->logger, "TensorBlock({})::acquire_read_access() failed as there is already processes that lock this block with write privileges.", m_descriptor.uuid);
        METHAN_INVALID_STATE;
    }

    if(m_state == ETensorBlockState::Garbage)
    {
        METHAN_LOG_ERROR(context()->logger, "TensorBlock({})::acquire_read_access() failed as the block is marked as garbage", m_descriptor.uuid);
        METHAN_INVALID_STATE;
    }
    
    if(m_state == ETensorBlockState::Future)
    {
        METHAN_LOG_ERROR(context()->logger, "TensorBlock({})::acquire_read_access() failed as the block is marked as future", m_descriptor.uuid);
        METHAN_INVALID_STATE;
    }
    METHAN_ASSERT_ARGUMENT(m_read_access + 1 > m_read_access);
#endif

    m_read_access++;
    m_state = ETensorBlockState::ReadLocked;
    return m_data_block->handle();
}

METHAN_API Methan::DataBlock::PtrType Methan::TensorBlock::acquire_write_access()
{
    std::lock_guard guard(m_mutex);
#ifdef METHAN_EXPAND_ASSERTION
    if(m_read_access > 0 || m_write_access > 0)
    {
        METHAN_LOG_ERROR(context()->logger, "TensorBlock({})::acquire_write_access() failed as there is already processes that lock this block with read privileges.", m_descriptor.uuid);
        METHAN_INVALID_STATE;
    }

    if(m_state == ETensorBlockState::Garbage)
    {
        METHAN_LOG_ERROR(context()->logger, "TensorBlock({})::acquire_write_access() failed as the block is marked as garbage", m_descriptor.uuid);
        METHAN_INVALID_STATE;
    }
#endif

    m_write_access++;
    m_state = ETensorBlockState::WriteLocked;
    return m_data_block->handle();
}

METHAN_API void Methan::TensorBlock::mark_as_garbage()
{
    std::lock_guard guard(m_mutex);
#ifdef METHAN_EXPAND_ASSERTION
    if(m_read_access > 0 || m_write_access > 0)
    {
        METHAN_LOG_ERROR(context()->logger, "TensorBlock({})::mark_as_garbage() failed as there is already processes that lock this block with read privileges.", m_descriptor.uuid);
        METHAN_INVALID_STATE;
    }
#endif

    m_state = ETensorBlockState::Garbage;
}

METHAN_API void Methan::TensorBlock::release_read_access()
{
    std::lock_guard guard(m_mutex);
    METHAN_ASSERT_ARGUMENT(m_read_access > 0);
    if(--m_read_access == 0)
    {
        m_state = ETensorBlockState::Freezing;
    }
}

METHAN_API void Methan::TensorBlock::release_write_access()
{
    std::lock_guard guard(m_mutex);
    METHAN_ASSERT_ARGUMENT(m_write_access == 1);
    m_state = ETensorBlockState::Freezing;
}

