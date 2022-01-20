#pragma once

#include <vector>

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

    struct DeviceDescriptor
    {
        Uuid uuid;
        EDeviceType deviceType;
        uint32_t core; /// number of thread that can be executed in parrallel

        float floops; // MHz
        float freq;   // MHz

        METHAN_SERDE_GENERATOR(DeviceDescriptor, uuid, deviceType, core, floops, freq);
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

        inline const Uuid& uuid() const
        {
            return m_descriptor.uuid;
        }

        inline const std::vector<Uuid>& memories() const
        {
            return m_memories;
        }

    protected:
        DeviceDescriptor m_descriptor;
        std::vector<Uuid> m_memories; // List of all the memory that this device can access

    private:
        std::string m_name;
    };


}
