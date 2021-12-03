#pragma once

#include <methan/core/except.hpp>


namespace Methan {

    template<typename T>
    inline T lerp(T t, T t0, T t1)
    {
        return (T(1) - t) * t0 + t * t1;
    }

}
