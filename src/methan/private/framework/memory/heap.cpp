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
    m_descriptor.alignement = 0;
    m_descriptor.maxAllocationCount = std::numeric_limits<DataSize>::max();
    m_descriptor.memoryType = EMemoryType::CpuHeap;
    
#ifdef METHAN_OS_WINDOWS
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof(statex);
    GlobalMemoryStatusEx(&statex);
    m_descriptor.maxUsage = (DataSize) statex.ullAvailPhys;
#endif
    
#ifdef METHAN_OS_UNIX_LIKE
    long page_size = sysconf(_SC_PAGE_SIZE);
#if defined(METHAN_OS_LINUX)
    long pages = sysconf(_SC_AVPHYS_PAGES);
#elif defined(METHAN_OS_MACOS)
    #warning "heap.cpp: As MacOS do not support _SC_AVPHYS_PAGES the heap maximum memory describe in the MemoryDescriptor is the maximum amount of memory."
    long pages = sysconf(_SC_PHYS_PAGES);
#endif
    m_descriptor.maxUsage = (DataSize) pages * (DataSize) page_size;
#endif

    METHAN_LOG_INFO(context->logger, "Max-Physical-Memory: {}", to_string(m_descriptor.maxUsage));

    if(m_descriptor.maxUsage > maxMemoryUsage)
    {
        m_descriptor.maxUsage = maxMemoryUsage;
    }

    // Create the allocator
    m_allocator = new HeapAllocator(context, this);
    m_heapFlowFactory = new HeapFlowFactory(context, this);
}

METHAN_API Methan::Heap::~Heap()
{
    delete m_heapFlowFactory;
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

METHAN_API Methan::HeapFlowFactory::HeapFlowFactory(Context context, Heap* heap)
: AbstractDataFlowFactory(context, heap->uuid(), heap->uuid())
{
    // m_descriptor.flags = EDataFlowPoliciesFlag::SupportAbort;
}

METHAN_API Methan::HeapFlowFactory::~HeapFlowFactory()
{

}

METHAN_API Methan::AbstractDataFlow* Methan::HeapFlowFactory::__create_flow(DataBlock* source,
                                                                            DataBlock* destination,
                                                                            std::vector<FlowPosition> sourceSites,
                                                                            std::vector<FlowPosition> destinationSites,
                                                                            const Uuid& uuid)
{
    return new HeapFlow(context(), source, destination, sourceSites, destinationSites, this, uuid);
}

METHAN_API Methan::HeapFlow::HeapFlow(Context context,
                                      DataBlock* source,
                                      DataBlock* destination,
                                      std::vector<FlowPosition> sourceSites,
                                      std::vector<FlowPosition> destinationSites,
                                      HeapFlowFactory* factory,
                                      const Uuid& uuid)
: AbstractDataFlow(context,
                   source,
                   destination,
                   sourceSites, 
                   destinationSites,
                   factory,
                   uuid,
                   "HeapFlow")
{ }

METHAN_API Methan::HeapFlow::~HeapFlow()
{

}

METHAN_API Methan::EDataFlowStateFlags Methan::HeapFlow::__run()
{
    // Note of optimisation idea
    // 1. If elementSize is a multiple of 1, 2, 4, 8 we may use the corresponding int type to trivially copy it faster
    //    namely 'uint8_t', 'uint16_t', 'uint32_t', 'uint64_t'

    for(size_t i = 0; i < m_sourceSites.size(); ++i)
    {
        if(m_sourceSites[i].is_trivially_copyable() && m_destinationSites[i].is_trivially_copyable())
        {
            memcpy(reinterpret_cast<uint8_t*>(std::get<void*>(destination()->handle())) + m_destinationSites[i].offset,
                   reinterpret_cast<const uint8_t*>(std::get<void*>(source()->handle())) + m_sourceSites[i].offset,
                   m_sourceSites[i].length * m_sourceSites[i].elementSize);
        }
        else
        {
            uint64_t sourceOffset = m_sourceSites[i].offset;
            uint64_t destinationOffset = m_destinationSites[i].offset;

            for(DataSize j = 0; j < m_sourceSites[i].length; ++j)
            {
                memcpy(reinterpret_cast<uint8_t*>(std::get<void*>(destination()->handle())) + destinationOffset,
                       reinterpret_cast<const uint8_t*>(std::get<void*>(source()->handle())) + sourceOffset,
                       m_sourceSites[i].elementSize);

                sourceOffset += m_sourceSites[i].stride;
                destinationOffset += m_destinationSites[i].stride;
            }
        }
    }

    // Do not forget to mark the operation as terminated !!
    return EDataFlowStateFlag::Terminated;
}

