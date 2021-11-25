#include <methan/private/framework/memory/heap.hpp>
#include <methan/private/private_context.hpp>
#include <methan/utility/data_size.hpp>
#include <methan/private/private_formatter.hpp>
#include <limits>
#include <stdlib.h>

#ifdef METHAN_OS_WINDOWS
#define NOMINMAX
#include <Windows.h>
#endif

#ifdef METHAN_OS_UNIX_LIKE
#include <unistd.h>
#endif

METHAN_API Methan::Heap::Heap(Context context, DataSize maxMemoryUsage)
: AbstractMemory(context, "heap")
{
    m_memoryDescriptor.alignement = 0;
    m_memoryDescriptor.maxAllocationCount = std::numeric_limits<DataSize>::max();
    m_memoryDescriptor.memoryType = EMemoryType::CpuHeap;
    
#ifdef METHAN_OS_WINDOWS
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof(statex);
    GlobalMemoryStatusEx(&statex);
    m_memoryDescriptor.maxUsage = (DataSize) statex.ullAvailPhys;
#endif
    
#ifdef METHAN_OS_UNIX_LIKE
    long page_size = sysconf(_SC_PAGE_SIZE);
#if defined(METHAN_OS_LINUX)
    long pages = sysconf(_SC_AVPHYS_PAGES);
#elif defined(METHAN_OS_MACOS)
    #warning "heap.cpp: As MacOS do not support _SC_AVPHYS_PAGES we heap maximum memory describe in the MemoryDescriptor is the maximum amount of memory."
    long pages = sysconf(_SC_PHYS_PAGES);
#endif
    m_memoryDescriptor.maxUsage = (DataSize) pages * (DataSize) page_size;
#endif

    METHAN_LOG_INFO(context->logger, "Max-Physical-Memory: {}", to_string(m_memoryDescriptor.maxUsage));

    if(m_memoryDescriptor.maxUsage > maxMemoryUsage)
    {
        m_memoryDescriptor.maxUsage = maxMemoryUsage;
    }

    // Setup the flag that defines the capabilities of that memory
    m_memoryDescriptor.capabilitiesFlag =
        EMemoryCapabilitiesFlag::SupportKeepAllocationView;
    
    // Create the allocator
    m_allocator = new HeapAllocator(context, this);
}

METHAN_API Methan::Heap::~Heap()
{
    delete (HeapAllocator*) m_allocator;
}

METHAN_API Methan::HeapAllocator::HeapAllocator(Context context, Heap* heap)
: AbstractAllocator(context, heap)
{

}

METHAN_API Methan::HeapAllocator::~HeapAllocator()
{

}

METHAN_API bool Methan::HeapAllocator::__alloc(DataSize size, DataBlock::PtrType* result)
{
    void* ptr = ::malloc((size_t) size);
    if(ptr == nullptr) return true;
    *result = ptr;
    return false;
}

METHAN_API bool Methan::HeapAllocator::__free(DataBlock::PtrType* result)
{
    ::free(std::get<void*>(*result));
    return false;
}
