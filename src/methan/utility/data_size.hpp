#pragma once

#include <string>

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

    /**
     * @brief Convert the size to string and append to the end the corresponding suffix
     * 'GB', 'MB', 'KB', 'B'
     * 
     * @param dataSize the size of the data to be converted to string
     * @return std::string the formatted equivalence of the input dataSize
     */
    METHAN_API std::string to_string(DataSize dataSize);
    
}