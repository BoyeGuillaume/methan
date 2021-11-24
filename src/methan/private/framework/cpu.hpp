#pragma once

#include <methan/core/except.hpp>
#include <methan/private/framework/device.hpp>

namespace Methan {

    class Cpu : public AbstractDevice
    {
        METHAN_DISABLE_COPY_MOVE(Cpu);
    
    public:
        METHAN_API Cpu(Context context, uint8_t maxCoreUsed);
        METHAN_API ~Cpu();
    };
    
}
