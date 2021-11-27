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

METHAN_API std::string Methan::to_string(DType type) noexcept
{
    switch (type)
    {
    case DType::Int8:
        return "dtype.int8";
        
    case DType::Int16:
        return "dtype.int16";
        
    case DType::Int32:
        return "dtype.int32";
        
    case DType::Int64:
        return "dtype.int64";
        
    case DType::Uint8:
        return "dtype.uint8";
        
    case DType::Uint16:
        return "dtype.uint16";
        
    case DType::Uint32:
        return "dtype.uint32";
        
    case DType::Uint64:
        return "dtype.uint64";
        
    case DType::Float16:
        return "dtype.float16";
        
    case DType::Float32:
        return "dtype.float32";
        
    case DType::Float64:
        return "dtype.float64";
        
    default:
        return "NaN";
    }
}
