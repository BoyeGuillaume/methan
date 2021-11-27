#include <methan/core/dtype.hpp>
#include <methan/utility/foreach.hpp>

METHAN_API Methan::DataSize Methan::size_of(DType type) noexcept
{
    switch (type)
    {
    case DType::Int8:
    case DType::Uint8:
        return 1;

    case DType::Int16:
    case DType::Uint16:
    case DType::Float16:
        return 2;

    case DType::Int32:
    case DType::Uint32:
    case DType::Float32:
        return 4;

    case DType::Int64:
    case DType::Uint64:
    case DType::Float64:
        return 8;
    
    default:
        return 0;
    }
}

// Kinda hacky but i don't copy that by hey...
#define HELPER_ITERATOR(type, it)    METHAN_CONCATENATE(METHAN_CONCATENATE(case DType::,type),it): return "dtype." METHAN_STRINGIZE(METHAN_CONCATENATE(type,it));

METHAN_API std::string Methan::to_string(DType type) noexcept
{
    switch (type)
    {
        METHAN_FOR_EACH_P1(HELPER_ITERATOR, Int, 8, 16, 32, 64)
        METHAN_FOR_EACH_P1(HELPER_ITERATOR, Uint, 8, 16, 32, 64)
        METHAN_FOR_EACH_P1(HELPER_ITERATOR, Float, 16, 32, 64)

    default:
        return "NaN";
    }
}
