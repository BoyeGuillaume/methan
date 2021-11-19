#include <methan/utility/exception.hpp>

METHAN_API Methan::Exception::Exception(const std::string& what, const char* file, size_t line, ExceptionType type)
: m_file(file),
m_line(line),
m_type(type)
{
    m_what = to_string(m_type) + "Exception : " + what + 
             " [file=\"" + std::string(file) + "\", line=" + std::to_string(line) + "] ";

}

METHAN_API const char* Methan::Exception::what() const noexcept
{
    return m_what.c_str();
}

METHAN_API std::string Methan::to_string(ExceptionType type)
{
    switch (type)
    {
    case ExceptionType::IllegalArgument:
        return "IllegalArgument";
    case ExceptionType::IllegalState:
        return "IllegalState";
    case ExceptionType::NullPointer:
        return "NullPointer";
    case ExceptionType::AlreadyInitialized:
        return "AlreadyInitialized";
    case ExceptionType::IndexOutOfBounds:
        return "IndexOutOfBounds";
    case ExceptionType::BadCastException:
        return "BadCastException";
    default:
        return "Unknown";
    }
}

