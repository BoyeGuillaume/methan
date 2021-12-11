#pragma once

#include <methan/core/except.hpp>
#include <methan/core/tensor_node.hpp>
#include <methan/utility/string_identifier.hpp>

namespace Methan::__private__
{
    class TensorNodeImpl 
    {
        METHAN_DISABLE_COPY_MOVE(TensorNodeImpl);

    public:
        METHAN_API TensorNodeImpl(const std::vector<TensorNodeImplPtr>& dependencies,
            StringIdentifier op_id);
        
        inline const std::vector<TensorNodeImplPtr>& dependencies() const 
        {
            return m_dependencies;
        }

        inline StringIdentifier op_id() const
        {
            return m_op_id;
        }

    private:
        std::vector<TensorNodeImplPtr> m_dependencies;
        StringIdentifier m_op_id;
    };
}
