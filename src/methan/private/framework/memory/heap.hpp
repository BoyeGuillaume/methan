#pragma once

#include <methan/core/except.hpp>
#include <methan/private/framework/memory/memory.hpp>
#include <methan/private/framework/memory/allocator.hpp>

namespace Methan {

    class Heap : public AbstractMemory
    {
        METHAN_DISABLE_COPY_MOVE(Heap);

    public:
        METHAN_API Heap(Context context, DataSize maxMemoryUsage);
        METHAN_API ~Heap();
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

}
