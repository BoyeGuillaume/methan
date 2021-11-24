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

        float floops; // MHz
        float freq;   // MHz

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

    class AbstractDevice : public Contextuable
    {
        METHAN_DISABLE_COPY_MOVE(AbstractDevice);

    protected:
        METHAN_API AbstractDevice(Context context, std::string name);

    public:
        METHAN_API virtual ~AbstractDevice();

        inline const DeviceDescriptor& descriptor() const
        {
            return m_descriptor;
        }

        inline const std::string& name() const
        {
            return m_name;
        }

        inline const Uuid& uuid()
        {
            return m_descriptor.uuid;
        }

    protected:
        DeviceDescriptor m_descriptor;

    private:
        std::string m_name;
    };


}
