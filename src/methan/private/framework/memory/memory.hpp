#pragma once

#include <vector>

#include <methan/core/except.hpp>
#include <methan/utility/uuid.hpp>
#include <methan/core/contextuable.hpp>
#include <methan/core/serializable.hpp>
#include <methan/utility/enum.hpp>
#include <methan/private/framework/framework.hpp>


namespace Methan {
    
    enum class EMemoryType
    {
        CpuHeap,
        GpuShared,
        GpuOnly,
        Swap
    };

    enum class EMemoryCapabilitiesFlag : uint8_t
    {
        SupportAsyncIO = 1 << 0,
        SupportKeepAllocationView = 1 << 1,
    };

    typedef EnumFlag<EMemoryCapabilitiesFlag> EMemoryCapabilitiesFlags;
    METHAN_ENUMSET_OPERATORS(EMemoryCapabilitiesFlags);

    struct MemoryDescriptor
    {
        Uuid uuid;
        EMemoryType memoryType;
        EMemoryCapabilitiesFlags capabilitiesFlag;
        uint8_t alignement; // If alignement (> 256) ...
        DataSize maxUsage;
        uint64_t maxAllocationCount;

        METHAN_SERDE_GENERATOR(MemoryDescriptor,
            uuid,
            memoryType,
            capabilitiesFlag,
            alignement,
            maxUsage,
            maxAllocationCount);
    };

    class AbstractMemory : public Contextuable
    {
        METHAN_DISABLE_COPY_MOVE(AbstractMemory);
    
    protected:
        METHAN_API AbstractMemory(Context context, std::string name);

    public:
        METHAN_API virtual ~AbstractMemory();

        inline const std::string& name() const
        {
            return m_name;
        }

        inline const Uuid& uuid() const
        {
            return m_memoryDescriptor.uuid;
        }

        inline const MemoryDescriptor& descriptor() const noexcept
        {
            return m_memoryDescriptor;
        }

        inline AbstractAllocator* allocator() const noexcept
        {
            return m_allocator;
        }

    protected:
        MemoryDescriptor m_memoryDescriptor;
        AbstractAllocator* m_allocator;

    private:
        std::string m_name;
    };

}

