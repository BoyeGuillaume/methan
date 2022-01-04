#pragma once

#include <string>
#include <vector>
#include <variant>
#include <unordered_map>

#include <methan/core/except.hpp>
#include <methan/core/contextuable.hpp>
#include <methan/private/framework/framework.hpp>
#include <methan/private/framework/framework.hpp>
#include <methan/private/framework/operator/operator_factory.hpp>
#include <methan/utility/uuid.hpp>
#include <methan/utility/string_identifier.hpp>
#include <methan/utility/enum.hpp>

#define METHAN_REGISTER_OP_FACTORY(factory)                                                         \
    __METHAN_REGISTER_OPERATOR_FACTORY_DETAILS(factory, METHAN_UNIQUE_IDENTIFIER(op_factory_))

#define __METHAN_REGISTER_OPERATOR_FACTORY_DETAILS(factory, identifier)                             \
    static int identifier =                                                                         \
    []() {                                                                                          \
        Methan::OperatorRegistry::__register([](Context context){ return new factory(context); });  \
        return 0;                                                                                   \
    }()

#define METHAN_OP_FACTORY(opFactoryName) \
    METHAN_OP_FACTORY_DETAILS(opFactoryName, METHAN_CONCATENATE(METHAN_CONCATENATE(op_factory_generated_, opFactoryName), ___unique_identifier_without_counter))

#define METHAN_OP_FACTORY_DETAILS(opFactoryName, identifier)                                        \
    class opFactoryName : public Methan::AbstractOperatorFactory {                                  \
    public:                                                                                         \
        METHAN_API opFactoryName(Methan::Context context);                                          \
                                                                                                    \
        METHAN_API OpDependencyDescriptor* get_op_dependencies(const std::vector<size_t>& input_ranks, const std::vector<size_t>& output_ranks, const std::vector<Parameter>& parameters) override; \
        METHAN_API std::optional<std::vector<TensorShape>> inferred_result_shape(const std::vector<TensorShape*>& inputs, const std::vector<Parameter>& parameters) const override; \
    protected:                                                                                      \
        METHAN_API AbstractOperator* __create_operator(const std::vector<TensorBlock*>& inputs, const std::vector<TensorBlock*>& outputs, const std::vector<Parameter>& parameters, const OpCreateDescriptor& descriptor) override; \
    };                                                                                              \
    static constexpr Methan::OpFactoryBuilderHelper identifier = Methan::OpFactoryBuilderHelper()   \

#define METHAN_OP_FACTORY_DEFINE(opFactoryName) \
    METHAN_OP_FACTORY_DEFINE_DETAILS(opFactoryName, METHAN_CONCATENATE(METHAN_CONCATENATE(op_factory_generated_, opFactoryName), ___unique_identifier_without_counter))

#define METHAN_OP_FACTORY_DEFINE_DETAILS(opFactoryName, identifier)                                 \
    METHAN_API opFactoryName::opFactoryName(Methan::Context context) : Methan::AbstractOperatorFactory(context, METHAN_IDENTIFIER(identifier.name), identifier.flags) {}

namespace Methan {

    struct OpFactoryBuilderHelper
    {
        constexpr OpFactoryBuilderHelper() : flags(), name("") {}
        constexpr OpFactoryBuilderHelper(EOpFactoryFlags flags, const char* name) : flags(flags), name(name) {}

        constexpr OpFactoryBuilderHelper withFlags(EOpFactoryFlags flags_)
        {
            return OpFactoryBuilderHelper(flags_ | flags, name);
        }

        constexpr OpFactoryBuilderHelper withName(const char* name)
        {
            return OpFactoryBuilderHelper(flags, name);
        }

        EOpFactoryFlags flags;
        const char* name;
    };

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

        METHAN_API static int __register(const std::function<AbstractOperatorFactory*(Context)>& _constr);

    private:
        std::unordered_map<StringIdentifier, AbstractOperatorFactory*> m_factories;
    };
 
}