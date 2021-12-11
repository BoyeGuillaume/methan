#pragma once

#include <memory>
#include <vector>

#include <methan/core/except.hpp>
#include <methan/core/weak_tensor.hpp>




namespace Methan::__private__
{
    class TensorNodeImpl;
    typedef std::shared_ptr<TensorNodeImpl> TensorNodeImplPtr;
}

namespace Methan
{
    class TensorNode 
    {
        using TensorNodeImplPtr = __private__::TensorNodeImplPtr;

    public: 
    private:
        METHAN_API TensorNode(TensorNodeImplPtr self);

        TensorNodeImplPtr m_self;
    };
}