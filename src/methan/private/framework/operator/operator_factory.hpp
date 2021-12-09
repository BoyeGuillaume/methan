#pragma once

#include <string>

#include <methan/core/except.hpp>
#include <methan/core/context.hpp>
#include <methan/core/contextuable.hpp>
#include <methan/private/framework/framework.hpp>
#include <methan/utility/uuid.hpp>
#include <methan/utility/string_identifier.hpp>
#include <methan/utility/enum.hpp>
#include <methan/utility/uuid.hpp>
#include <methan/private/framework/operator/operator_registery.hpp>

#define METHAN_REGISTER_OP_FACTORY(opFactoryName)                                        \
    METHAN_EXPAND(__METHAN_REGISTER_OPERATOR_FACTORY(opFactoryName))

namespace Methan {

    class AbstractOperatorFactory : public Contextuable
    {
        METHAN_DISABLE_COPY_MOVE(AbstractOperatorFactory);

    public:
        METHAN_API AbstractOperatorFactory(Context context, const StringIdentifier& identifier);
        METHAN_API virtual ~AbstractOperatorFactory();

        inline const StringIdentifier& identifier() const
        {
            return m_identifier;
        }

        inline const std::string& name() const noexcept
        {
            return m_name;
        }

        METHAN_API bool is_valid(const std::vector<TensorBlock*>& blocks) const;
        METHAN_API AbstractOperator* create_operator(const Uuid& uuid, const std::vector<TensorBlock*>& blocks);

    protected:
        virtual AbstractOperator* __create_operator(const Uuid& uuid, const std::vector<TensorBlock*>& blocks) = 0;
        virtual bool __is_valid(const std::vector<TensorBlock*>& blocks) const = 0;

    private:
        StringIdentifier m_identifier;
        std::string m_name;
    };

}