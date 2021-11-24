#include <methan/private/framework/memory/heap.hpp>
#include <methan/private/private_context.hpp>
#include <methan/utility/data_literals.hpp>
#include <limits>

#ifdef METHAN_OS_WINDOWS
#define NOMINMAX
#include <Windows.h>
#endif

#ifdef METHAN_OS_UNIX_LIKE
#include <unistd.h>
#endif

METHAN_API Methan::Heap::Heap(Context context, uint64_t maxMemoryUsage)
: AbstractMemory(context, "heap")
{
    m_memoryDescriptor.alignement = 0;
    m_memoryDescriptor.maxAllocationCount = std::numeric_limits<uint64_t>::max();
    m_memoryDescriptor.memoryType = EMemoryType::CpuHeap;
    
#ifdef METHAN_OS_WINDOWS
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof(statex);
    GlobalMemoryStatusEx(&statex);
    m_memoryDescriptor.maxUsage = (uint64_t) statex.ullAvailPhys;
#endif
    
#ifdef METHAN_OS_UNIX_LIKE
    long page_size = sysconf(_SC_PAGE_SIZE);
    long pages = get_avphys_pages();
    m_memoryDescriptor.maxUsage = (uint64_t) pages * (uint64_t) page_size;
#endif

    METHAN_LOG_INFO(context->logger, "Max-Physical-Memory: {} MB", m_memoryDescriptor.maxUsage / 1_MB);

    if(m_memoryDescriptor.maxUsage > maxMemoryUsage)
    {
        m_memoryDescriptor.maxUsage = maxMemoryUsage;
    }

    // Setup the flag that defines the capabilities of that memory
    m_memoryDescriptor.capabilitiesFlag =
        EMemoryCapabilitiesFlag::SupportKeepAllocationView;
}

METHAN_API Methan::Heap::~Heap()
{
    
}
