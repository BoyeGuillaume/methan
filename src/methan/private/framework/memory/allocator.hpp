#pragma once

#include <mutex>
#include <variant>

#include <methan/core/except.hpp>
#include <methan/core/contextuable.hpp>
#include <methan/core/serializable.hpp>
#include <methan/utility/uuid.hpp>
#include <methan/utility/data_size.hpp>
#include <methan/private/framework/framework.hpp>
#include <methan/private/framework/memory/memory.hpp>

namespace Methan {

    struct DataBlockDescriptor
    {
        Uuid uuid;
        Uuid memory;
        DataSize size;

        METHAN_SERDE_GENERATOR(DataBlockDescriptor, uuid, memory, size);
    };


    class DataBlock : public Contextuable
    {
        METHAN_DISABLE_COPY_MOVE(DataBlock);
        friend class AbstractAllocator;
        METHAN_API DataBlock(Context context, AbstractAllocator* allocator);

    public:
        using PtrType = std::variant<std::monostate, void*>;

        METHAN_API ~DataBlock();

        METHAN_API PtrType handle();

        inline DataSize size() const noexcept
        {
            return is_weak() ? 0ULL : m_descriptor.size;
        }

        inline bool is_weak() const noexcept
        {
            return m_flag & 0x01;
        }

        inline Uuid uuid() const noexcept
        {
            return m_descriptor.uuid;
        }

        inline AbstractAllocator* reference_allocator() const noexcept
        {
            return m_allocator;
        }

    private:
        AbstractAllocator* m_allocator;
        DataBlockDescriptor m_descriptor;
        PtrType m_handle;
        uint8_t m_flag;
    };


    class AbstractAllocator : public Contextuable
    {
        METHAN_DISABLE_COPY_MOVE(AbstractAllocator);
        
    protected:
        METHAN_API AbstractAllocator(Context context, AbstractMemory* memory);
        METHAN_API virtual ~AbstractAllocator();

    public:
        inline Uuid uuid() const noexcept
        {
            return m_uuid;
        }

        inline AbstractMemory* memory() const noexcept
        {
            return m_memory;
        }

        _NODISCARD METHAN_API DataBlock* alloc(DataSize size);
        METHAN_API void free(DataBlock* block);

        inline DataSize cumulated_usage() const noexcept
        {
            return m_cumulated_usage;
        }

        inline DataSize current_usage() const noexcept
        {
            return m_current_usage;
        }

        inline DataSize max_usage() const noexcept
        {
            return m_memory->descriptor().maxUsage;
        }

    protected:
        /**
         * @brief This abstract function should allocate the required size and return the result in the PtrType pointer. Notice that
         * this methods is GUARENTED to be syncronously called accross multiple-threads. (also won't be called in the same time of __free)
         * 
         * @param size the size of the data to be called
         * @param result the resulting type
         * @return true if any error occured during the allocation process
         * @return false if no error occured
         */
        virtual bool __alloc(DataSize size, DataBlock::PtrType* result) = 0;
        
        /**
         * @brief This abstract function should free a given PtrType pointer. Notice that this methods is GUARENTED to be syncro,ously called
         * accross multiple-threads. (also won't be called in the same time of __alloc)
         * 
         * @param result The pointer resulting of an allocation on the same allocator that must now be free. NOTICE THAT ANY DATA THAT IS FREE
         * MAY THEN BE REALLOCATED LATER ON !!
         * 
         * @return true if any error occured during the allocation process
         * @return false if no error occured
         */
        virtual bool __free(DataBlock::PtrType* result) = 0;

    private:
        DataSize m_current_usage;
        DataSize m_cumulated_usage;
        AbstractMemory* m_memory;
        Uuid m_uuid;
        std::mutex m_mutex;
    };

}
