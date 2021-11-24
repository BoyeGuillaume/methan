#pragma once

#include <methan/core/except.hpp>
#include <methan/private/framework/memory/memory.hpp>

namespace Methan {

    class Heap : public AbstractMemory
    {
        METHAN_DISABLE_COPY_MOVE(Heap);

    public:
        METHAN_API Heap(Context context, uint64_t maxMemoryUsage);
        METHAN_API ~Heap();
    };

}
