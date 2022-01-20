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

        virtual Uuid kernel_uuid() const = 0;

    protected:
        METHAN_API AbstractOperator(AbstractOperatorFactory* factory, Uuid uuid, const std::vector<TensorBlock*>& inputs, const std::vector<TensorBlock*>& outputs);
        METHAN_API virtual ~AbstractOperator();

    private:
        AbstractOperatorFactory* m_factory;
        std::vector<TensorBlock*> m_inputs;
        std::vector<TensorBlock*> m_outputs;
    };

}

