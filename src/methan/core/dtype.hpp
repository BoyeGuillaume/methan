#pragma once

#include <string>

#include <methan/core/except.hpp>
#include <methan/utility/data_size.hpp>


namespace Methan {

    enum class DType : uint8_t
    {
        Int8,
        Int16,
        Int32,
        Int64,
        Uint8,
        Uint16,
        Uint32,
        Uint64,
        Float16,
        Float32,
        Float64
    };

    METHAN_API DataSize size_of(DType type) noexcept;
    METHAN_API std::string to_string(DType type) noexcept;

}