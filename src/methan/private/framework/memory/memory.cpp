#include <algorithm>

#include <methan/private/framework/memory/memory.hpp>
#include <methan/private/private_context.hpp>
#include <methan/private/private_formatter.hpp>
#include <methan/private/framework/memory/allocator.hpp>


METHAN_API Methan::AbstractMemory::AbstractMemory(Context context, std::string name)
: Contextuable(context),
  m_name(std::move(name)),
  m_allocator(nullptr)
{
    m_memoryDescriptor.uuid = generate_uuid(context);
    METHAN_LOG_DEBUG(context->logger, "AbstractMemory::AbstractMemory(\"{}\", {}) registered", m_name, m_memoryDescriptor.uuid);
    context->memories.push_back(this);
}

METHAN_API Methan::AbstractMemory::~AbstractMemory()
{
    METHAN_LOG_DEBUG(context()->logger, "AbstractMemory::AbstractMemory(\"{}\", {}) destroyed", m_name, m_memoryDescriptor.uuid);
    auto rIterator = std::remove(context()->memories.begin(), context()->memories.end(), this);
    context()->memories.erase(rIterator, context()->memories.end());
}

