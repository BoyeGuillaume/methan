#pragma once

#include <methan/core/except.hpp>
#include <methan/core/contextuable.hpp>
#include <methan/core/tensor_node.hpp>
#include <methan/utility/string_identifier.hpp>
#include <methan/private/framework/framework.hpp>

namespace Methan::__private__ {
    
    class TensorNodeImpl : public Contextuable
    {
        METHAN_DISABLE_COPY_MOVE(TensorNodeImpl);

    public:
        METHAN_API TensorNodeImpl(StringIdentifier identifier, const std::vector<TensorNodeImplPtr>& dependencies, AbstractOperatorFactory* operator_factory);

        inline const std::vector<TensorNodeImplPtr>& dependencies() const
        {
            return m_dependencies;
        }

        inline AbstractOperatorFactory* operator_factory() const
        {
            return m_operator_factory;
        }

        inline StringIdentifier id() const
        {
            return m_id;
        }

    private:
        std::vector<TensorNodeImplPtr> m_dependencies;
        AbstractOperatorFactory* m_operator_factory;
        StringIdentifier m_id;
    };

}