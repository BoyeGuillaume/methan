#include <methan/private/framework/memory/dataflow.hpp>
#include <methan/private/private_context.hpp>
#include <methan/private/private_formatter.hpp>

METHAN_API Methan::AbstractDataFlow::AbstractDataFlow(Context context,
                                                      DataBlock* source,
                                                      DataBlock* destination,
                                                      std::vector<FlowPosition> sourceSites,
                                                      std::vector<FlowPosition> destinationSites,
                                                      AbstractDataFlowFactory* factory,
                                                      Uuid uuid,
                                                      const std::string& name)
: AbstractTask(context, uuid, name),
  m_source(source),
  m_destination(destination),
  m_sourceSites(sourceSites),
  m_destinationSites(destinationSites),
  m_factory(factory)
{

}

METHAN_API Methan::AbstractDataFlow::~AbstractDataFlow()
{
    if(running())
    {
        METHAN_LOG_ERROR(context()->logger, "AbstractDataFlow() destructor called while operation(s) was still in progress !!");
    }
}


METHAN_API uint32_t Methan::AbstractDataFlow::run() 
{
    EDataFlowStateFlags result = 
        EDataFlowStateFlag::Initiate;

    m_source->acquire_safe_read_access();
    m_destination->acquire_safe_write_access();

    signal()->signal(result);
    METHAN_LOG_DEBUG(context()->logger, "DataFlow({}) initiated", uuid());

    try {
        result |= __run();
        result |= EDataFlowStateFlag::Terminated;
        METHAN_LOG_DEBUG(context()->logger, "DataFlow({}) successfully terminated", uuid());
    }
    catch(std::exception e) {
        METHAN_LOG_ERROR(context()->logger, "DataFlow({})::__run() failed with error {}", uuid(), e.what());
        
        result |= EDataFlowStateFlag::ErrorState;
    }

    // Reliese the read / write access acquire beforehand    
    m_source->release_read_access();
    m_destination->release_write_access();

    return (uint32_t) result;
}

METHAN_API void Methan::AbstractDataFlow::abort()
{
#ifdef METHAN_EXPAND_ASSERTION
    if(!(m_factory->descriptor().flags & EDataFlowPoliciesFlag::SupportAbort))
    {
        METHAN_LOG_ERROR(context()->logger, "DataFlow({})::abort() called on a non-abortable DataFlow({})", uuid(), m_factory->uuid());
        METHAN_INVALID_STATE;
    }
#endif

    uint32_t flag = state();
    if(!(flag & (uint32_t) EDataFlowStateFlag::Terminated)) return;
    METHAN_ASSERT(flag & (uint32_t) EDataFlowStateFlag::Initiate, Methan::ExceptionType::IllegalArgument, "AbstractDataFlow::abort() may not be called when the flow has not yet been started");
    
    __abort();
}

METHAN_API void Methan::AbstractDataFlow::__abort()
{
    METHAN_INVALID_STATE;
}

METHAN_API Methan::AbstractDataFlowFactory::AbstractDataFlowFactory(Context context, Uuid source, Uuid destination)
: Contextuable(context)
{
    m_descriptor.source = source;
    m_descriptor.destination = destination;
    m_descriptor.uuid = generate_uuid(context);
    context->flowsFactories[std::make_pair(source, destination)] = this;
    METHAN_LOG_DEBUG(context->logger, "New DataFlowFactory({}) between {} and {} registered", m_descriptor.uuid, m_descriptor.source, m_descriptor.destination);
}

METHAN_API Methan::AbstractDataFlowFactory::~AbstractDataFlowFactory()
{
    METHAN_LOG_DEBUG(context()->logger, "DataFlowFactory({}) deleted");
    context()->flowsFactories.erase(Methan::UuidPair(source(), destination()));
}

METHAN_API Methan::AbstractDataFlow* Methan::AbstractDataFlowFactory::initiate_flow(DataBlock* source, DataBlock* destination, std::vector<FlowPosition> sourceSites, std::vector<FlowPosition> destinationSites, const Uuid& uuid)
{
#ifdef METHAN_EXPAND_ASSERTION
    METHAN_ASSERT_ARGUMENT(sourceSites.size() == destinationSites.size());

    if(m_descriptor.source != source->reference_allocator()->uuid() ||
       m_descriptor.destination != destination->reference_allocator()->uuid())
    {
        METHAN_LOG_ERROR(context()->logger, "Cannot use the DataFlowFactory({}) to copy from {} to {}", m_descriptor.uuid, source->reference_allocator()->memory()->name(),
                                                                                                                           destination->reference_allocator()->memory()->name());
        METHAN_INVALID_STATE;
    }

    for(size_t i = 0; i < destinationSites.size(); ++i)
    {
        METHAN_ASSERT_ARGUMENT(sourceSites[i].elementSize == destinationSites[i].elementSize && destinationSites[i].elementSize != 0);
        METHAN_ASSERT_ARGUMENT(sourceSites[i].is_valid(source->size()));
        METHAN_ASSERT_ARGUMENT(destinationSites[i].is_valid(destination->size()));
        METHAN_ASSERT_ARGUMENT(sourceSites[i].length == destinationSites[i].length);
    }
#endif

    std::lock_guard guard(__lock_flow_creation_m);
    AbstractDataFlow* flow = __create_flow(source, destination, std::move(sourceSites), std::move(destinationSites), uuid);
    METHAN_LOG_DEBUG(context()->logger, "New DataFlow({}) from {} to {}", flow->uuid(), source->reference_allocator()->memory()->name(), destination->reference_allocator()->memory()->name());    
    return flow;
}
