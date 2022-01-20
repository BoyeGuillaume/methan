#pragma once

#include <string>
#include <optional>
#include <vector>
#include <variant>
#include <functional>
#include <unordered_map>

#include <methan/core/except.hpp>
#include <methan/core/context.hpp>
#include <methan/core/contextuable.hpp>
#include <methan/core/serializable.hpp>
#include <methan/core/tensor_shape.hpp>
#include <methan/utility/uuid.hpp>
#include <methan/utility/enum.hpp>
#include <methan/utility/matrix.hpp>
#include <methan/utility/string_identifier.hpp>
#include <methan/private/framework/framework.hpp>
#include <methan/private/framework/operator/parameter.hpp>
#include <methan/utility/math.hpp>


namespace Methan {

    enum class EOpFactoryFlag : uint32_t
    {
        /**
         * @brief A mask to only select the kernel support of a given operator. (last 4 bits)
         */
        KernelSupportMask = 0xf,

        /**
         * @brief Whever or not the selected factory support the kernel cpu
         */
        KernelSupportCpu = 0x1,
    };

    typedef EnumFlag<EOpFactoryFlag> EOpFactoryFlags;
    METHAN_ENUMSET_OPERATORS(EOpFactoryFlags);

    struct OpDescriptor
    {
        StringIdentifier identifier;
        EOpFactoryFlags flags;
    };

    enum class EOpDependency
    {
        /**
         * @brief The operationg REQUIRED all component along the input axis to be defined.
         * @warning This dependency, is easy to define but disable the optimizer from ever splitting this operation and
         * therefore should be avoid when possible
         */
        Everything,

        /**
         * @brief Perform the operation only if all `k` component in the input tensor are correctly defined.
         * 
         * @paragraph Exemple Given a tensor (10, 10, 10) ~> (5, 5, 5) width constraint {{kNearestNeighbor, 2, 0}}. This only constraint enable one to split
         * the operation into (?, ?, x1:x2) ~ (x1-2:x1+2, ?, ?)
         */
        kNearestNeighbor,

        /**
         * @brief Perform the operation component-wise along the axis. Require both axis to have the same size. 
         * 
         * @paragraph Exemple Given a tensor (5, 10, 15) ~> (9, 15, 10) with constraint {{ComponentWise, 2, 1}} we require that 15 == 15. Also
         * this only constraint enable one to split the operation into (?, ?, x1:x2) ~ (?, x1:x2, ?)
         */
        ComponentWise
    };

    struct OpDependencyNearestNeighborEXT
    {
        uint32_t k;
        
        /**
         * @brief define the behavior at the boundary of the tensor
         * 
         * Extrema:
         *  * `padding = 0`: There is no padding, therefore the output_size should be equal to the input_size - k + 1
         *  * `padding >= k`: invalid state
         * 
         */
        uint32_t padding;
    };

    struct OpDependencyCoordinateDescriptor
    {
        // Notice that in some condition the `output_axis` is not relevent 
        size_t input_axis, output_axis;
        EOpDependency type;
        std::variant<std::monostate, OpDependencyNearestNeighborEXT> ext;
    };

    struct OpCreateDescriptor
    {
        Uuid device_uuid;
        Uuid op_uuid;

        METHAN_SERDE_GENERATOR(OpCreateDescriptor, op_uuid, device_uuid);
    };

    typedef Matrix<std::vector<OpDependencyCoordinateDescriptor>> OpDependencyDescriptor;

    class AbstractOperatorFactory : public Contextuable
    {
        METHAN_DISABLE_COPY_MOVE(AbstractOperatorFactory);

    public:
        METHAN_API AbstractOperatorFactory(Context context, const StringIdentifier& identifier, EOpFactoryFlags flags, uint8_t number_parameters, uint32_t parameters_descriptor_usage_mask);
        METHAN_API virtual ~AbstractOperatorFactory();

        inline const StringIdentifier& identifier() const
        {
            return m_descriptor.identifier;
        }

        inline const EOpFactoryFlags flags() const
        {
            return m_descriptor.flags;
        }

        inline const OpDescriptor* descriptor() const 
        {
            return &m_descriptor;
        }

        inline const std::string& name() const noexcept
        {
            return m_name;
        }

        /**
         * @brief Get the dependencies for a given inputs ranks and output ranks.
         * @warning This object MUST not be deallocate by the use as it is manager by the inner working of the operator to
         * prevent allocating many of them.
         * @warning This method is not thread safe
         * 
         * @param input_ranks a list containing the ranks of each inputs
         * @param output_ranks a list containing the ranks of each outputs
         * @param parameters a list of all the parameters used to construct the operator
         * @return const OpDependencyDescriptor* a pointer to an descriptor that MUST NOT be released.
         */
        METHAN_API const OpDependencyDescriptor* get_op_dependencies(const std::vector<size_t>& input_ranks, const std::vector<size_t>& output_ranks, const std::vector<Parameter>& parameters);

        /**
         * @brief Given the input of a AbstractOperatorFactory return optionally the inferred output
         * 
         * @param inputs a list of the input tensor shapes
         * @param parameters a list of parameters passed to the operator upon construction
         * @return std::optional<std::vector<TensorShape>> a list of the inferred outputs tensor shape
         */
        virtual std::optional<std::vector<TensorShape>> inferred_result_shape(const std::vector<TensorShape*>& inputs, const std::vector<Parameter>& parameters) const = 0;
        
        /**
         * @brief Whever or not we can create an operator with the given inputs and outputs tensor block
         * @warning This method is not thread safe
         * 
         * @param inputs inputs tensor block
         * @param outputs outputs tensor block
         * @param parameters a list of parameters passed to the operator upon construction
         * @return bool whever or not this is a valid split
         */
        METHAN_API bool is_valid(const std::vector<SlicedTensorShape>& inputs, const std::vector<SlicedTensorShape>& outputs, const std::vector<Parameter>& parameters);

        /**
         * @brief Create a operator object given a configuration
         * @warning This method is not thread safe
         * 
         * @param inputs the inputs given to the operator
         * @param outputs the outputs given to the operator
         * @param parameters a list of all the parameters passed to the operator upon construction
         * @param create_description a struct holding parameters for the creation of this operator
         * @return AbstractOperator* the newly created object
         */
        METHAN_API AbstractOperator* create_operator(const std::vector<TensorBlock*>& inputs, const std::vector<TensorBlock*>& outputs, const std::vector<Parameter>& parameters, const OpCreateDescriptor& create_descriptor);

    protected:
        virtual AbstractOperator* __create_operator(const std::vector<TensorBlock*>& inputs, const std::vector<TensorBlock*>& outputs, const std::vector<Parameter>& parameters, const OpCreateDescriptor& create_descriptor) = 0;
        
        /**
         * @brief Create the dependencies given a configuration
         * @note This method ain't thread safe and should not therefore be concieved to be thread safe
         * 
         * @param allocated_memory an already-allocated-non-constructed object where the new object should be located. To construct the object one can use the little known new syntax
         * `new (allocated_memory) OpDependencyDescriptor(...);`
         * @param input_ranks a list of the input ranks
         * @param output_ranks a list of the output ranks
         * @param parameters a list of parameters. Please note that ONLY parameters masked by `parameters_descriptor_usage_mask` should be used.
         * @return true if an error occured an the input given to this function is invalid
         * @return false everything happen has expected
         */
        virtual bool __create_new_op_dependencies(OpDependencyDescriptor* allocated_memory, const std::vector<size_t>& input_ranks, const std::vector<size_t>& output_ranks, const std::vector<Parameter>& parameters) = 0;

    private:
        struct OpDependenciesTableEntry
        {
            OpDependencyDescriptor* ptr;
            std::vector<Parameter> parameters;
        };

        const uint8_t m_number_parameters;
        const uint32_t m_parameters_descriptor_usage_mask;
        OpDescriptor m_descriptor;
        std::string m_name;
        std::unordered_map<std::pair<std::vector<size_t>, std::vector<size_t>>, std::vector<OpDependenciesTableEntry>> m_op_dependencies;
    };

}
