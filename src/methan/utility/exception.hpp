#pragma once

#include <exception>
#include <string>

#include <methan/core/except.hpp>

#define METHAN_THROW_EXCEPTION(what, type)                                     \
    throw Methan::Exception(what, METHAN_EXPAND(__FILE__), METHAN_EXPAND(__LINE__), type)

#define METHAN_INVALID_STATE                                                   \
    METHAN_THROW_EXCEPTION("No description was provided", Methan::ExceptionType::IllegalState)

namespace Methan {

    /**
     * @brief A list of all the exception that are thrown in the Methan API
     */
    enum class ExceptionType
    {
        IllegalArgument,
        IllegalState,
        NullPointer,
        AlreadyInitialized,
        IndexOutOfBounds,
        BadCastException,
        HashCollision,
        NotEnoughMemory,
        Unknown
    };

    /**
     * @brief Convert the enumeration `ExceptionType` to a string representation
     * 
     * @param type the input enum type
     * @return std::string the corresponding string
     */
    METHAN_API std::string to_string(ExceptionType type);

    /**
     * @brief Basic exception class used throughout the methan API
     */
    class Exception : public std::exception
    {
    public:
        /**
         * @brief Create an exception without any description
         */
        inline Exception(std::nullptr_t)
        : Exception("No description was provided", "", -1)
        {}

        /**
         * @brief Create an exception with information about the exception
         * 
         * @param what fast description of the exception and why the exception occured
         * @param file the file where the exception was raised
         * @param line the line where the exception was raised
         * @param type the type of the exception that was raised
         */
        METHAN_API Exception(const std::string& what, const char* file, size_t line, ExceptionType type = ExceptionType::Unknown);

        /**
         * @brief Return an enhanced description of the error using the file & line at which the error occured
         * Notice that the pointer is valid as long as the Exception object still alive.
         * 
         * @return const* char
         */
        METHAN_API const char* what() const noexcept override;

        /**
         * @brief Return the file where the exception occured
         * 
         * @return const char* the where the exception occured
         */
        inline const char* file() const noexcept 
        {
            return m_file;
        }

        /**
         * @brief Return the line where the exception occured
         * 
         * @return the line where the exception occured
         */
        inline size_t line() const noexcept
        {
            return m_line;
        }

        /**
         * @brief Return the exception type
         * 
         * @return the type of the exception raised
         */
        inline ExceptionType type() const noexcept
        {
            return m_type;
        }

    private:
        std::string m_what;
        const char* m_file;
        size_t m_line;
        ExceptionType m_type;
    };

}

