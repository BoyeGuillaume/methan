#pragma once

#include <methan/core/except.hpp>
#include <methan/core/tensor_shape.hpp>
#include <methan/core/contextuable.hpp>
#include <methan/utility/uuid.hpp>
#include <methan/private/framework/framework.hpp>
#include <methan/core/dtype.hpp>


namespace Methan {


    class Tensor : public Contextuable
    {
        METHAN_DISABLE_COPY_MOVE(Tensor);
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

    private:
        TensorShape m_shape;
        Uuid m_uuid;
        DType m_dtype;
    };

}
