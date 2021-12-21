#include <methan/private/framework/operator/operator_factory.hpp>
#include <methan/private/private_context.hpp>
#include <methan/private/private_formatter.hpp>
#include <methan/utility/assertion.hpp>
#include <methan/private/framework/tensor/tensor_block.hpp>

METHAN_API Methan::AbstractOperatorFactory::AbstractOperatorFactory(Context context, const StringIdentifier& identifier)
: Contextuable(context),
  m_identifier(identifier)
{
    m_name = "Operator{" + std::to_string(identifier) + "}";
}

METHAN_API Methan::AbstractOperatorFactory::~AbstractOperatorFactory()
{
    METHAN_LOG_DEBUG(context()->logger, "OperatorFactory(\"{}\") was destructed", std::to_string(m_identifier));
}

METHAN_API Methan::AbstractOperator* Methan::AbstractOperatorFactory::create_operator(const Uuid& uuid, const std::vector<TensorBlock*>& inputs, const std::vector<TensorBlock*>& outputs, const std::vector<Parameter>& parameters)
{
#ifdef METHAN_EXPAND_ASSERTION
    std::vector<SlicedTensorShape> inputs_;
    std::vector<SlicedTensorShape> outputs_;
    inputs_.reserve(inputs.size());
    outputs_.reserve(outputs.size());

    for(size_t i = 0; i < inputs.size(); ++i)
    {
        METHAN_ASSERT_NON_NULL(inputs[i]);
        inputs_.push_back(inputs[i]->shape());
    }

    for(size_t i = 0; i < outputs.size(); ++i)
    {
        METHAN_ASSERT_NON_NULL(outputs[i]);
        outputs_.push_back(outputs[i]->shape());
    }

    if(!is_valid(inputs_, outputs_, parameters))   
    {
        METHAN_LOG_ERROR(context()->logger, "OperatorFactory(\"{}\")::create_operator() failed as the ::is_valid(inputs) return false", std::to_string(m_identifier));
        METHAN_THROW_EXCEPTION("Invalid argument exception, cannot call this operator with this arguments", ExceptionType::IllegalArgument);
    }
#endif

    AbstractOperator* ops = __create_operator(uuid, inputs, outputs, parameters);
    METHAN_ASSERT_NON_NULL(ops); 

    return ops;
}

METHAN_API bool Methan::AbstractOperatorFactory::is_valid(const std::vector<SlicedTensorShape>& inputs, const std::vector<SlicedTensorShape>& outputs, const std::vector<Parameter>& parameters)
{
    // Retrieve the operator dependencies for a given input_ranks & output_ranks
    std::vector<size_t> input_ranks(inputs.size(), 0);
    std::vector<size_t> output_ranks(outputs.size(), 0);

    for(size_t i = 0; i < inputs.size(); ++i)
        input_ranks[i] = inputs[i].rank();

    for(size_t i = 0; i < outputs.size(); ++i)
        output_ranks[i] = outputs[i].rank();

    const OpDependencyDescriptor* descriptors = get_op_dependencies(input_ranks, output_ranks, parameters);

    // Require the operator to be valid
    METHAN_ASSERT_NON_NULL(descriptors);

    // Assert that the operator is working correctly
    for(size_t i = 0; i < inputs.size(); ++i)
    {
        for(size_t j = 0; j < outputs.size(); ++j)
        {
            // Retrieve the descriptor between the output[j] and the input[i]
            const std::vector<OpDependencyCoordinateDescriptor>& coordinate_descriptor = descriptors->get(i, j);

            // Determine whever or not all condition for a given input & output are correct
            for(size_t k = 0; k < coordinate_descriptor.size(); ++k)
            {
                const OpDependencyCoordinateDescriptor& c = coordinate_descriptor[k];
                METHAN_ASSERT_INDEX(c.input_axis, inputs[i].rank());
                METHAN_ASSERT_INDEX(c.output_axis, outputs[i].rank());

                const EOpDependency type = c.type;
                if(type == EOpDependency::Everything)
                {
                    if(inputs[i].shape()[c.input_axis] != inputs[i].parent_shape()[c.input_axis]) return false;
                }
                else if(type == EOpDependency::ComponentWise)
                {
                    const uint32_t input_shape = inputs[i].shape()[c.input_axis];
                    const uint32_t output_shape = outputs[j].shape()[c.output_axis];
                    const uint32_t input_offset = inputs[i].offsets()[c.input_axis];
                    const uint32_t output_offset = outputs[i].offsets()[c.output_axis];

                    if(inputs[i].parent_shape()[c.input_axis] != outputs[j].parent_shape()[c.output_axis]) return false;
                    if(input_offset > output_offset) return false;
                    if(input_offset + input_shape < output_offset + output_shape) return false;
                }
                else if(type == EOpDependency::kNearestNeighbor)
                {
                    const OpDependencyNearestNeighborEXT& ext = std::get<OpDependencyNearestNeighborEXT>(c.ext);
                    METHAN_ASSERT_ARGUMENT(ext.k > 0);
                    METHAN_ASSERT_ARGUMENT(ext.padding < ext.k);

                    const uint32_t input_shape = inputs[i].shape()[c.input_axis];
                    const uint32_t output_shape = outputs[j].shape()[c.output_axis];
                    const uint32_t input_offset = inputs[i].offsets()[c.input_axis];
                    const uint32_t output_offset = outputs[i].offsets()[c.output_axis];
                    const uint32_t delta = ext.k - ext.padding; // => delta > 0

                    if(inputs[i].parent_shape()[c.input_axis] != outputs[j].parent_shape()[c.output_axis] + 2 * delta) return false;
                    if(input_offset > output_offset) return false;
                    if(input_offset + input_shape < output_offset + output_shape + 2 * delta) return false;
                }
                else
                {
                    METHAN_INVALID_STATE;
                }
            }
        }
    }

    // If we made it so far, then we can simply return true as the parameters is correct
    return true;
}
