#pragma once

#include <methan/core/except.hpp>

namespace Methan {

    typedef uint64_t DataSize;

    constexpr DataSize operator"" _B(unsigned long long _Val) noexcept {
        return _Val;
    }

    constexpr DataSize operator"" _KB(unsigned long long _Val) noexcept {
        return 1000 * _Val;
    }

    constexpr DataSize operator"" _MB(unsigned long long _Val) noexcept {
        return 1000000 * _Val;
    }

    constexpr DataSize operator"" _GB(unsigned long long _Val) noexcept {
        return 1000000000 * _Val;
    }

}