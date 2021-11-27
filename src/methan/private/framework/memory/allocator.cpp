#include <methan/private/framework/memory/allocator.hpp>
#include <methan/private/private_context.hpp>
#include <methan/private/private_formatter.hpp>
#include <methan/private/framework/memory/memory.hpp>

METHAN_API Methan::DataBlock::DataBlock(Context context, AbstractAllocator* allocator)
: Contextuable(context),
  m_allocator(allocator),
  m_handle(std::monostate{}),
  m_flags(EDataBlockStateFlag::Weak),
  m_operations(__CurrentOperations{0U, 0U})
{ }

METHAN_API Methan::DataBlock::~DataBlock()
{
    if(!is_weak())
    {
        m_allocator->free(this);
    }
}

METHAN_API Methan::DataBlock::PtrType Methan::DataBlock::handle()
{
    METHAN_ASSERT(!is_weak(), Methan::ExceptionType::NullPointer, "DataBlock::handle() failed as the pointer is in weak state (as already been deleted)");
    return m_handle;
}

METHAN_API bool Methan::DataBlock::try_acquire_safe_read_access()
{
    std::lock_guard guard(__operation_lock_m);
    METHAN_ASSERT(!is_weak(), Methan::ExceptionType::NullPointer, "DataBlock::try_acquire_safe_read_access() failed as the pointer is in weak state");
    if(m_operations.operatingWrite != 0) return false;
    m_operations.operatingRead++;
    return true;
}

METHAN_API bool Methan::DataBlock::try_acquire_safe_write_access()
{
    std::lock_guard guard(__operation_lock_m);
    METHAN_ASSERT(!is_weak(), Methan::ExceptionType::NullPointer, "DataBlock::try_acquire_safe_write_access() failed as the pointer is in weak state");
    if(m_operations.raw != 0) return false;
    m_operations.operatingWrite++;
    return true;
}

METHAN_API void Methan::DataBlock::acquire_safe_read_access()
{
    if(!try_acquire_safe_read_access())
    {
        METHAN_LOG_ERROR(context()->logger, "DataBlock::acquire_safe_read_access() failed");
        METHAN_THROW_EXCEPTION("DataBlock::acquire_safe_read_access() failed", ExceptionType::IllegalState);
    }
}

METHAN_API void Methan::DataBlock::acquire_safe_write_access()
{
    if(!try_acquire_safe_write_access())
    {
        METHAN_LOG_ERROR(context()->logger, "DataBlock::acquire_safe_write_access() failed");
        METHAN_THROW_EXCEPTION("DataBlock::acquire_safe_write_access() failed", ExceptionType::IllegalState);
    }
}

METHAN_API void Methan::DataBlock::acquire_read_access()
{
    std::lock_guard guard(__operation_lock_m);
    METHAN_ASSERT(!is_weak(), Methan::ExceptionType::NullPointer, "DataBlock::acquire_read_access() failed as the pointer is in weak state");
    m_operations.operatingRead++;
}

METHAN_API void Methan::DataBlock::acquire_write_access()
{
    std::lock_guard guard(__operation_lock_m);
    METHAN_ASSERT(!is_weak(), Methan::ExceptionType::NullPointer, "DataBlock::acquire_write_access() failed as the pointer is in weak state");
    m_operations.operatingWrite++;
}

METHAN_API void Methan::DataBlock::release_read_access()
{
    std::lock_guard guard(__operation_lock_m);
    METHAN_ASSERT(!is_weak(), Methan::ExceptionType::NullPointer, "DataBlock::release_read_access() failed as the pointer is in weak state");

#ifdef METHAN_EXPAND_ASSERTION
    if(m_operations.operatingRead == 0)
    {
        METHAN_LOG_ERROR(context()->logger, "DataBlock::release_read_access() called when no read-operating was registered");
        METHAN_INVALID_STATE;
    }
#endif

    m_operations.operatingRead--;
}

METHAN_API void Methan::DataBlock::release_write_access()
{
    std::lock_guard guard(__operation_lock_m);
    METHAN_ASSERT(!is_weak(), Methan::ExceptionType::NullPointer, "DataBlock::release_read_access() failed as the pointer is in weak state");

#ifdef METHAN_EXPAND_ASSERTION
    if(m_operations.operatingWrite == 0)
    {
        METHAN_LOG_ERROR(context()->logger, "DataBlock::release_write_access() called when no write-operating was registered");
        METHAN_INVALID_STATE;
    }
#endif

    m_operations.operatingWrite--;
}

METHAN_API Methan::AbstractAllocator::AbstractAllocator(Context context, AbstractMemory* memory)
: Contextuable(context),
  m_memory(memory),
  m_uuid(memory->uuid()),
  m_cumulated_usage(0x0),
  m_current_usage(0x0)
{
    METHAN_ASSERT_NON_NULL(memory);
    METHAN_LOG_DEBUG(context->logger, "New allocator for memory {} registered", memory->name());
}

METHAN_API Methan::AbstractAllocator::~AbstractAllocator()
{
    METHAN_LOG_DEBUG(context()->logger, "Allocator for memory {} destroyed", m_memory->name());
    METHAN_LOG_INFO(context()->logger, "Cumumulated-Memory-Usage for memory {}", to_string(m_cumulated_usage));

    std::lock_guard guard(m_mutex);
    if(m_current_usage != 0)
    {
        METHAN_LOG_WARNING(context()->logger, "Memory leak detected in {} (used memory during destruction of allocator is {})", m_memory->name(), m_current_usage);
    }
}

[[nodiscard]] METHAN_API Methan::DataBlock* Methan::AbstractAllocator::alloc(DataSize size)
{
    std::lock_guard guard(m_mutex);
    if(size + m_current_usage < m_current_usage || size + m_current_usage > max_usage())
    {
        METHAN_LOG_ERROR(context()->logger, "Allocation failure as cannot allocate {} size into memory {} because of the upper memory bounds sets to {}", to_string(size), m_memory->name(), to_string(max_usage()));
        METHAN_THROW_EXCEPTION("Memory Exceeded. Cannot allocated required size", ExceptionType::NotEnoughMemory);
    }

    Methan::DataBlock* dataBlock = new Methan::DataBlock(context(), this);
    dataBlock->m_descriptor.memory = m_memory->uuid();
    dataBlock->m_descriptor.size = size;
    dataBlock->m_descriptor.uuid = generate_uuid(context());
    
    if (__alloc(size, &dataBlock->m_handle))
    {
        delete dataBlock;
        METHAN_LOG_ERROR(context()->logger, "Allocation failure for size={}, memory={}, allocator={}", to_string(size), m_memory->name(), m_uuid);
        return nullptr;
    }

    METHAN_LOG_INFO(context()->logger, "Allocation of {} of memory by {} onto {} successfull", to_string(size), m_uuid, m_memory->name());

    m_current_usage += size;
    m_cumulated_usage += size;
    dataBlock->m_flags &= ~EDataBlockStateFlag::Weak; 
    return dataBlock;
}

METHAN_API void Methan::AbstractAllocator::free(DataBlock* dataBlock)
{
    std::lock_guard guard(m_mutex);
    METHAN_ASSERT_NON_NULL(dataBlock);
#ifdef METHAN_EXPAND_ASSERTION
    if(dataBlock->is_weak())
    {
        METHAN_LOG_ERROR(context()->logger, "Cannot free DataBlock({}) as this block is weak.", dataBlock->uuid());
        METHAN_THROW_EXCEPTION("Cannot free weak datablock", ExceptionType::NullPointer);
    }

    if(dataBlock->reference_allocator() != this)
    {
        METHAN_LOG_ERROR(context()->logger, "Cannot free DataBlock({}) by Allocator({}) as only Allocator({}) can free this block", dataBlock->uuid(), m_uuid, dataBlock->m_allocator->m_uuid);
        METHAN_THROW_EXCEPTION("DataBlock must be deallocated by the reference allocator", ExceptionType::IllegalArgument);
    }
#endif

    if (__free(&dataBlock->m_handle))
    {
        METHAN_LOG_ERROR(context()->logger, "Free failure for DataBlock({}) by Allocator({})", dataBlock->uuid(), uuid());
        METHAN_THROW_EXCEPTION("Failure in AbstractAllocator::free", ExceptionType::Unknown);
    }

    // Decrease the current usage
    METHAN_LOG_DEBUG(context()->logger, "DataBlock {} of size {} was free successfully", dataBlock->uuid(), to_string(dataBlock->size()));
    m_current_usage -= dataBlock->size();
    
    // Set the datablock to weak
    dataBlock->m_flags |= EDataBlockStateFlag::Weak;
    dataBlock->m_handle = std::monostate{};
    dataBlock->m_allocator = nullptr;
    dataBlock->m_descriptor.size = 0;
}