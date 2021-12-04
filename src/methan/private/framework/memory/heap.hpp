#pragma once

#include <methan/core/except.hpp>
#include <methan/private/framework/memory/memory.hpp>
#include <methan/private/framework/memory/allocator.hpp>
#include <methan/private/framework/memory/dataflow.hpp>

namespace Methan {

    class HeapFlowFactory;

    class Heap : public AbstractMemory
    {
        METHAN_DISABLE_COPY_MOVE(Heap);

    public:
        METHAN_API Heap(Context context, DataSize maxMemoryUsage);
        METHAN_API ~Heap();
    
    private:
        HeapFlowFactory* m_heapFlowFactory;
    };

    class HeapAllocator : public AbstractAllocator
    {
        METHAN_DISABLE_COPY_MOVE(HeapAllocator);
        METHAN_API HeapAllocator(Context context, Heap* heap);
        METHAN_API ~HeapAllocator();
        friend class Heap;

    protected:
        METHAN_API virtual bool __alloc(DataSize size, DataBlock::PtrType* result) override;
        METHAN_API virtual bool __free(DataBlock::PtrType* result) override;
    };

    class HeapFlowFactory : public AbstractDataFlowFactory
    {
        METHAN_DISABLE_COPY_MOVE(HeapFlowFactory);
        METHAN_API HeapFlowFactory(Context context, Heap* heap);
        METHAN_API ~HeapFlowFactory();
        friend class Heap;

    protected:
        METHAN_API AbstractDataFlow* __create_flow(DataBlock* source,
                                                   DataBlock* destination,
                                                   std::vector<FlowPosition> sourceSites,
                                                   std::vector<FlowPosition> destinationSites,
                                                   const Uuid& uuid);
    };

    class HeapFlow : public AbstractDataFlow
    {
        METHAN_DISABLE_COPY_MOVE(HeapFlow);
        METHAN_API HeapFlow(Context context,
                            DataBlock* source,
                            DataBlock* destination,
                            std::vector<FlowPosition> sourceSites,
                            std::vector<FlowPosition> destinationSites,
                            HeapFlowFactory* factory,
                            const Uuid& uuid);
        friend class HeapFlowFactory;

    protected:
        METHAN_API EDataFlowStateFlags __run() override;
        
    public:
        METHAN_API ~HeapFlow();
    };

}
