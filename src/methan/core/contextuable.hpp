#pragma once

#include <methan/core/except.hpp>
#include <methan/core/context.hpp>
#include <methan/utility/assertion.hpp>

namespace Methan {

    class Contextuable
    {
    public:
        inline Contextuable(Context context)
        : m_context(context)
        {
            METHAN_ASSERT_NON_NULL(context);
        }

        virtual ~Contextuable() = default;

        inline Context context() const noexcept
        {
            return m_context;
        }

    private:
        Context m_context;
    };

}
