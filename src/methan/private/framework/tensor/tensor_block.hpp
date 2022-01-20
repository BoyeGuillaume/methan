#pragma once

#include <mutex>

#include <methan/core/except.hpp>
#include <methan/core/contextuable.hpp>
#include <methan/core/tensor_shape.hpp>
#include <methan/utility/enum.hpp>
#include <methan/utility/uuid.hpp>
#include <methan/private/framework/tensor/tensor.hpp>
#include <methan/private/framework/framework.hpp>
#include <methan/private/framework/memory/allocator.hpp>

namespace Methan {

    enum class ETensorBlockState : uint32_t
    {
        /**
         * @brief An process (or multiple) is currently reading this block and therefore no writing operation
         * must occure
         */
        ReadLocked,

        /**
         * @brief An process (or multiple) is currently writing this block and therefore no reading operation (from other process
         * may occure)
         */
        WriteLocked,

        /**
         * @brief This block as not been yet complitly loaded (no process must read/write it)
         */
        Future,

        /**
         * @brief This block is currently available, neither used nor in construction. In summary nothing happen in here
         * 
         */
        Freezing,

        /**
         * @brief This block is marked as garbage. No reading/writing process must occurred. Once in this state accessing the block
         * will be Undefined Behavior.
         */
        Garbage
    };

    struct TensorBlockDescriptor
    {
        SlicedTensorShape sliced;
        Uuid tensor;
        Uuid uuid;

        inline TensorBlockDescriptor(Uuid tensor, Uuid uuid, SlicedTensorShape sliced) 
        : tensor(tensor),
          uuid(uuid),
          sliced(sliced)
        { }

        METHAN_SERDE_GENERATOR(TensorBlockDescriptor, sliced, tensor, uuid);
    };

    class TensorBlock : public Contextuable
    {
        METHAN_DISABLE_COPY_MOVE(TensorBlock);

    public:
        METHAN_API TensorBlock(Tensor* tensor, DataBlock* dataBlock, const TensorBlockDescriptor& descriptor);
        METHAN_API ~TensorBlock();

        METHAN_API DataBlock::PtrType acquire_read_access();
        METHAN_API DataBlock::PtrType acquire_write_access();
        METHAN_API void mark_as_garbage();
        METHAN_API void release_read_access();
        METHAN_API void release_write_access();

        inline const TensorBlockDescriptor& descriptor() const
        {
            return m_descriptor;
        }

        inline const SlicedTensorShape& shape() const
        {
            return m_descriptor.sliced;
        }

        inline ETensorBlockState state() const noexcept
        {
            return m_state;
        }

        inline Uuid uuid() const noexcept
        {
            return m_descriptor.uuid;
        }

        /**
         * @warning This is an methods is for possible usage of the lock internal to this object 
         * possibly externaly. This may not be used if one don't understand concequences. Notice 
         * that it may cause deadlock.
         */
        inline void __lock()
        {
            return m_mutex.lock();
        }

        /**
         * @warning This is an methods is for possible usage of the lock internal to this object 
         * possibly externaly. This may not be used if one don't understand concequences. Notice 
         * that it may cause deadlock.
         */
        inline void __unlock()
        {
            return m_mutex.unlock();
        }

    private:
        std::mutex m_mutex;
        uint16_t m_read_access;
        uint16_t m_write_access;
        ETensorBlockState m_state;
        TensorBlockDescriptor m_descriptor;
        Tensor* m_tensor;
        DataBlock* m_data_block;
    };

    // class TensorBlock : public Contextuable
    // {
    // public:
        
    // };

}
