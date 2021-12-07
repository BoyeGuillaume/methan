#pragma once

#include <vector>

#include <methan/core/except.hpp>
#include <methan/core/contextuable.hpp>
#include <methan/utility/string_identifier.hpp>
#include <methan/private/framework/framework.hpp>
#include <methan/private/framework/threading/task.hpp>


namespace Methan {

    class AbstractOperator : public AbstractTask
    {
    public:
        inline AbstractOperatorFactory* factory() const noexcept
        {
            return m_factory;
        }

    protected:
        METHAN_API AbstractOperator(AbstractOperatorFactory* factory, Uuid uuid, const std::vector<TensorBlock*>& blocks);
        METHAN_API virtual ~AbstractOperator();

    private:
        AbstractOperatorFactory* m_factory;
        std::vector<TensorBlock*> m_blocks;
    };

}

