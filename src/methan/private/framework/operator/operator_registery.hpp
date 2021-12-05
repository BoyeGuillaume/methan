#pragma once

#include <string>
#include <vector>
#include <variant>
#include <unordered_map>

#include <methan/core/except.hpp>
#include <methan/core/contextuable.hpp>
#include <methan/private/framework/framework.hpp>
#include <methan/utility/uuid.hpp>
#include <methan/utility/string_identifier.hpp>
#include <methan/utility/enum.hpp>
#include <methan/private/framework/framework.hpp>

#define __METHAN_REGISTER_OPERATOR_FACTORY(factory)                                                     \
    static volatile std::monostate __generated__##factory##__LINE__ =                                   \
        Methan::OperatorRegistry::__register(factory::id, [](Context context) { return new factory(context); });

namespace Methan {

    class OperatorRegistry : public Contextuable
    {
        METHAN_DISABLE_COPY_MOVE(OperatorRegistry);
        
    public:
        METHAN_API OperatorRegistry(Context context);
        METHAN_API ~OperatorRegistry();

        /**
         * @brief Find the operator with the given name if it exists. If no operator with that name exists
         * simply return nullptr
         * 
         * @param identifier the identifier of the given operator
         * @return AbstractOperatorFactory* a pointer to the corresponding factory if exists otherwise nullptr
         */
        METHAN_API AbstractOperatorFactory* find(const StringIdentifier& identifier) const;

        inline AbstractOperatorFactory* operator[](const StringIdentifier& identifier) const
        {
            return find(identifier);   
        }

        METHAN_API static std::monostate __register(const StringIdentifier& identifier, const std::function<AbstractOperatorFactory*(Context)>& _constr);

    private:
        std::unordered_map<StringIdentifier, AbstractOperatorFactory*> m_factories;
    };
 
}