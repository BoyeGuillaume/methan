#pragma once

#include <string>
#include <optional>
#include <vector>
#include <variant>
#include <functional>

#include <methan/core/except.hpp>
#include <methan/core/context.hpp>
#include <methan/core/contextuable.hpp>
#include <methan/private/framework/framework.hpp>
#include <methan/utility/uuid.hpp>
#include <methan/utility/string_identifier.hpp>
#include <methan/utility/enum.hpp>
#include <methan/utility/uuid.hpp>
#include <methan/utility/matrix.hpp>
#include <methan/private/framework/operator/parameter.hpp>
#include <methan/core/tensor_shape.hpp>
#include <methan/private/framework/operator/operator_registery.hpp>


#define METHAN_REGISTER_OP_FACTORY(opFactoryName)                                        \
    METHAN_EXPAND(__METHAN_REGISTER_OPERATOR_FACTORY(opFactoryName))

namespace Methan {

    enum class EOpDependency
    {
        Everything,
        kNearestNeighbor,
        SameComponent
    };

    struct OpDependencyNearestNeighborEXT
    {
        size_t k;
        size_t padding;
    };

    struct OpDependencyCoordinateDescriptor
    {
        // Notice that in some condition the `output_axis` is not relevent 
        size_t input_axis, output_axis;
        EOpDependency type;
        std::variant<std::monostate, OpDependencyNearestNeighborEXT> ext;
    };

    typedef Matrix<std::vector<OpDependencyCoordinateDescriptor>> OpDependencyDescriptor;


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

        /**
         * @brief Get the dependencies for a given inputs ranks and output ranks.
         * @warning This object MUST not be deallocate by the use as it is manager by the inner working of the operator to
         * prevent allocating many of them
         * 
         * @param input_ranks a list containing the ranks of each inputs
         * @param output_ranks a list containing the ranks of each outputs
         * @param parameters a list of all the parameters used to construct the operator
         * @return const OpDependencyDescriptor* a pointer to an descriptor that MUST NOT be released.
         */
        virtual const OpDependencyDescriptor* get_op_dependencies(const std::vector<size_t>& input_ranks, const std::vector<size_t>& output_ranks, const std::vector<Parameter>& parameters) = 0;

        /**
         * @brief Given the input of a AbstractOperatorFactory return optionally the inferred output
         * 
         * @param inputs a list of the input tensor shapes
         * @param parameters a list of parameters passed to the operator upon construction
         * @return std::optional<std::vector<TensorShape>> a list of the inferred outputs tensor shape
         */
        virtual std::optional<std::vector<TensorShape>> inferred_result(const std::vector<TensorShape*>& inputs, const std::vector<Parameter>& parameters) const = 0;
        
        /**
         * @brief Whever or not we can create an operator with the given inputs and outputs tensor block
         * 
         * @param inputs inputs tensor block
         * @param outputs outputs tensor block
         * @param parameters a list of parameters passed to the operator upon construction
         * @return bool whever or not this is a valid split
         */
        virtual bool is_valid(const std::vector<SlicedTensorShape>& inputs, const std::vector<SlicedTensorShape>& outputs, const std::vector<Parameter>& parameters) const = 0;

        /**
         * @brief Create a operator object given a configuration
         * 
         * @param uuid the uuid of the generated operator
         * @param inputs the inputs given to the operator
         * @param outputs the outputs given to the operator
         * @param parameters a list of all the parameters passed to the operator upon construction
         * @return AbstractOperator* the newly created object
         */
        METHAN_API AbstractOperator* create_operator(const Uuid& uuid, const std::vector<TensorBlock*>& inputs, const std::vector<TensorBlock*>& outputs, const std::vector<Parameter>& parameters);

    protected:
        virtual AbstractOperator* __create_operator(const Uuid& uuid, const std::vector<TensorBlock*>& inputs, const std::vector<TensorBlock*>& outputs, const std::vector<Parameter>& parameters) = 0;

    private:
        StringIdentifier m_identifier;
        std::string m_name;
    };

}