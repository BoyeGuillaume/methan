#pragma once

#include <methan/core/except.hpp>
#include <methan/utility/uuid.hpp>
#include <methan/core/contextuable.hpp>
#include <methan/core/serializable.hpp>

namespace Methan {
    
    enum class EDeviceType {
        Cpu,
        IntegratedGpu,
        DiscreteGpu
    };

    enum class EMemoryType {
        CpuHeap,
        GpuShared,
        GpuOnly,
        Swap
    };

    struct DeviceDescriptor
    {
        Uuid uuid;
        EDeviceType deviceType;
        uint32_t core; /// number of thread that can be executed in parrallel

        uint64_t floops;
        uint64_t freq;

        METHAN_SERDE_GENERATOR(DeviceDescriptor, uuid, deviceType, core, floops, freq);
    };

    struct MemoryDescriptor
    {
        Uuid uuid;
        EMemoryType memoryType;
        uint8_t alignement; // If alignement (> 256) ...
        uint64_t maxAllocationSize;
        uint64_t maxAllocationCount;

        METHAN_SERDE_GENERATOR(MemoryDescriptor, uuid, memoryType, alignement, maxAllocationSize, maxAllocationCount);
    };


    class Device : public Contextuable
    {
        METHAN_DISABLE_COPY_MOVE(Device);

    public:
        METHAN_API Device(Context context, DeviceDescriptor decriptor, std::string name);
        METHAN_API ~Device();

        inline const DeviceDescriptor& descriptor() const
        {
            return m_descriptor;
        }

        inline const std::string& name() const
        {
            return m_name;
        }

    private:
        DeviceDescriptor m_descriptor;
        std::string m_name;
    };


}
