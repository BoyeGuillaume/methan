#pragma once

#include <string>

#include <methan/core/except.hpp>
#include <methan/core/tensor_shape.hpp>
#include <methan/core/dtype.hpp>
#include <methan/utility/string_identifier.hpp>


namespace Methan
{
    class WeakTensor
    {
    public:
        METHAN_API WeakTensor(StringIdentifier id, const TensorShape& shape, DType type);

        inline const TensorShape& shape() const 
        {
            return m_shape;
        }

        inline DType type() const 
        {
            return m_type;
        }

        inline StringIdentifier id() const 
        {
            return m_id;
        }

        std::string to_string() const 
        {
            return "WeakTensor (id=" + std::to_string(m_id) + 
                ", shape=" + m_shape.to_string() + ")";
        }

    private:
        TensorShape m_shape;
        DType m_type;
        StringIdentifier m_id;
    };

}
