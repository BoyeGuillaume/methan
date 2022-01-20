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
        METHAN_API std::optional<std::vector<TensorShape>> inferred_result_shape(const std::vector<TensorShape*>& inputs, const std::vector<Parameter>& parameters) const override; \
                                                                                                    \
    protected:                                                                                      \
        METHAN_API AbstractOperator* __create_operator(const std::vector<TensorBlock*>& inputs, const std::vector<TensorBlock*>& outputs, const std::vector<Parameter>& parameters, const OpCreateDescriptor& create_descriptor) override; \
        METHAN_API bool __create_new_op_dependencies(OpDependencyDescriptor* allocated_memory, const std::vector<size_t>& input_ranks, const std::vector<size_t>& output_ranks, const std::vector<Parameter>& parameters) override; \
    };                                                                                              \
    static constexpr Methan::OpFactoryBuilderHelper identifier = Methan::OpFactoryBuilderHelper()   \

#define METHAN_OP_FACTORY_DEFINE(opFactoryName) \
    METHAN_OP_FACTORY_DEFINE_DETAILS(opFactoryName, METHAN_CONCATENATE(METHAN_CONCATENATE(op_factory_generated_, opFactoryName), ___unique_identifier_without_counter))

#define METHAN_OP_FACTORY_DEFINE_DETAILS(opFactoryName, identifier)                                 \
    METHAN_API opFactoryName::opFactoryName(Methan::Context context) : Methan::AbstractOperatorFactory(context, METHAN_IDENTIFIER(identifier.name), identifier.flags) {}

namespace Methan {

    struct OpFactoryBuilderHelper
    {
        constexpr OpFactoryBuilderHelper() noexcept
        : flags(),
          name(""),
          parameters_count(0x0),
          parameters_flag(0x0) {}

        constexpr OpFactoryBuilderHelper(EOpFactoryFlags flags, const char* name, uint8_t parameters_count, uint32_t parameters_flag) noexcept
        : flags(flags),
          name(name),
          parameters_flag(parameters_flag),
          parameters_count(parameters_count) {}

        /**
         * @brief Specify the flag used to configure the operator factory
         * 
         * @param flags_ an EOpFactoryFlags that is sets in the resulting factory
         * @return OpFactoryBuilderHelper a new instance of OpFactoryBuilderHelper
         * @note Notice that this method ain't optimize and is aimed to be constantly evaluate by the compiler
         */
        constexpr OpFactoryBuilderHelper with_flags(EOpFactoryFlags flags_) noexcept
        {
            return OpFactoryBuilderHelper(flags_ | flags, name, parameters_count, parameters_flag);
        }

        /**
         * @brief Specify the name of the current operator factory
         * 
         * @param name the name of the current operator factory
         * @return OpFactoryBuilderHelper a new instance of OpFactoryBuilderHelper
         * @note Notice that this method ain't optimize and is aimed to be constantly evaluate by the compiler
         */
        constexpr OpFactoryBuilderHelper with_name(const char* name) noexcept
        {
            return OpFactoryBuilderHelper(flags, name, parameters_count, parameters_flag);
        }

        /**
         * @brief Add a parameters to the list of parameters that MUST be passed in order to construct the operator factory. Notice
         * that even if the parameters ain't actively used to determine the shape it must be passed !!
         * 
         * @param index the index of the parameters
         * @param is_used_to_determine_shape whever or not this parameters is actively used to determine the shape of the result
         * @return OpFactoryBuilderHelper a new instance of OpFactoryBuilderHelper
         * @note Notice that this method ain't optimize and is aimed to be constantly evaluate by the compiler
         */
        constexpr OpFactoryBuilderHelper with_parameters(uint8_t index, bool is_used_to_determine_shape) noexcept
        {
            uint8_t count = index + 1;
            if (count < parameters_count) count = parameters_count;
            return OpFactoryBuilderHelper(flags, name, count, parameters_flag | ((uint8_t) (is_used_to_determine_shape) << index));
        }

        EOpFactoryFlags flags;
        const char* name;
        uint8_t parameters_count;
        uint32_t parameters_flag;
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