#include <queue>
#include <unordered_set>

#include <methan/private/framework/computational/tensor_node.hpp>
#include <methan/private/framework/computational/graph.hpp>
#include <methan/utility/assertion.hpp>
#include <methan/private/private_context.hpp>
#include <methan/private/private_formatter.hpp>

METHAN_API Methan::ComputationalGraph::ComputationalGraph(Context context, const std::vector<TensorNodeImplPtr>& outputs)
: Contextuable(context),
  m_outputs(outputs)
{
#ifdef METHAN_EXPAND_ASSERTION
    if (outputs.empty())
    {
        METHAN_LOG_ERROR(context->logger, "Cannot create ComputationalGraph without outputs nodes");
        METHAN_INVALID_STATE;
    }
#endif

    METHAN_LOG_DEBUG(context->logger, "New computational graph at {}", spdlog::fmt_lib::ptr(this));
    build_graph();
}

METHAN_API Methan::ComputationalGraph::~ComputationalGraph()
{
    METHAN_LOG_DEBUG(context()->logger, "Computation graph at {} deleted", spdlog::fmt_lib::ptr(this));
}

METHAN_API void Methan::ComputationalGraph::build_graph()
{
    std::queue<TensorNodeImplPtr> q;
    std::unordered_set<TensorNodeImpl*> discovered_nodes;

    for(auto& outputs : m_outputs)
    {
        discovered_nodes.insert(outputs.get());
        q.push(outputs);
    }
    
    while(!q.empty())
    {
        TensorNodeImplPtr node = q.front();
        q.pop();
        
        m_nodes.push_back(node);
        if (node->dependencies().empty())
        {
            m_inputs.push_back(node);
        }
        else
        {
            for(auto& parent : node->dependencies())
            {
                if(discovered_nodes.count(parent.get()) == 0)
                {
                    discovered_nodes.insert(parent.get());
                    q.push(parent);
                }
            }
        }
    }

    METHAN_LOG_INFO(context()->logger, "Computational graph built with {} nodes", m_nodes.size());
}

