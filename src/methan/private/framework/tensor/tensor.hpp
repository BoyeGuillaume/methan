#pragma once

#include <methan/core/except.hpp>
#include <methan/core/tensor_shape.hpp>
#include <methan/core/contextuable.hpp>
#include <methan/utility/uuid.hpp>
#include <methan/private/framework/framework.hpp>
#include <methan/core/dtype.hpp>
#include <mutex>


namespace Methan {

    class Tensor : public Contextuable
    {
        METHAN_DISABLE_COPY_MOVE(Tensor);
        friend class TensorBlock;
        
    public:
        METHAN_API Tensor(Context context, const Uuid& uuid, const TensorShape& shape, DType dtype);
        METHAN_API ~Tensor();

        inline const TensorShape& shape() const noexcept
        {
            return m_shape;
        }

        inline const Uuid& uuid() const noexcept
        {
            return m_uuid;
        }

        inline DType dtype() const noexcept
        {
            return m_dtype;
        }

        inline uint64_t size() const noexcept
        {
            return m_shape.size();
        }

        inline DataSize memsize() const noexcept
        {
            return size() * size_of(m_dtype);
        }

        inline const std::vector<Uuid>& blocks() const noexcept
        {
            return m_blocks;
        }

    private:
        std::vector<Uuid> m_blocks;
        std::mutex m_mutex;
        TensorShape m_shape;
        Uuid m_uuid;
        DType m_dtype;
    };

}
