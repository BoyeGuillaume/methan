#pragma once

#include <vector>
#include <memory>

#include <methan/core/except.hpp>
#include <methan/core/contextuable.hpp>
#include <methan/core/tensor_node.hpp>


namespace Methan {


    class ComputationalGraph : public Contextuable
    {
        METHAN_DISABLE_COPY_MOVE(ComputationalGraph);

    public:
        using TensorNodeImpl = Methan::__private__::TensorNodeImpl;
        using TensorNodeImplPtr = Methan::__private__::TensorNodeImplPtr;

        METHAN_API ComputationalGraph(Context context, const std::vector<TensorNodeImplPtr>& outputs);
        METHAN_API ~ComputationalGraph();

    private:
        METHAN_API void build_graph();

        std::vector<TensorNodeImplPtr> m_outputs;
        std::vector<TensorNodeImplPtr> m_inputs;
        std::vector<TensorNodeImplPtr> m_nodes;
    };

}
