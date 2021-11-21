#pragma once

#include <functional>
#include <vector>
#include <string>

#include <methan/core/except.hpp>
#include <methan/utility/varient.hpp>
#include <methan/utility/data_literals.hpp>

namespace Methan {

    enum class EComputerType : uint8_t {
        Observer,
        Master,
        Slave
    };

    enum class ELogLevel {
        Off,
        Critical,
        Error,
        Warning,
        Info,
        Debug
    };

    METHAN_OPAQUE_HANDLE(Context);

    /**
     * @brief Helper object that ease the creation of any context object. Notice that the Context object
     * is an opaque object that must be free once the context has been used using `void free(Context)`
     */
    class ContextBuilder
    {
        METHAN_DISABLE_COPY(ContextBuilder);
    public:
        /**
         * @brief Build the ContextBuilder object
         */
        METHAN_API ContextBuilder();
        
        /**
         * @brief Destruct the ContextBuilder object
         */
        METHAN_API ~ContextBuilder();

        /**
         * @brief Add a new output logger (corresponding to stdout)
         * 
         * @param logLevel The maximum log level corresponding to that output
         * @param supportColor Do we use color during the output
         * @return ContextBuilder& reference to the current ContextBuilder object 
         */
        METHAN_API ContextBuilder& add_logger_stdout(ELogLevel logLevel, bool supportColor = true);

        /**
         * @brief Add a new output logger corresponding to a given file
         * 
         * @param filename the name of the logger file
         * @param level the log level corresponding to that output
         * @return ContextBuilder& reference to the current ContextBuilder object
         */
        METHAN_API ContextBuilder& add_logger_file(const std::string& filename, ELogLevel level);

        /**
         * @brief Add a new output logger corresponding to a rotating file (group of `maxFiles` files that are write
         * one after the other until they reach the maxSize)
         * 
         * @param filename the name template for the rotating file
         * @param level the log level corresponding to that output
         * @param maxSize the maximum size that a file can takes
         * @param maxFiles the maximum number of file that can be written before overwrite the first file
         * @return ContextBuilder& reference to the current ContextBuilder object 
         */
        METHAN_API ContextBuilder& add_logger_rotating_file(const std::string& filename, ELogLevel level, DataSize maxSize = 4_KB, size_t maxFiles = 3);

        /**
         * @brief Set the computer type the context is corresponding to. May be Master/Client/Observer. Default is (master)
         * 
         * @param computerType The new computer type
         * @return METHAN_API& 
         */
        METHAN_API ContextBuilder& self_as(EComputerType computerType);

        /**
         * @brief Construct the Context object based on the current configuration. May result in an error if the given configuration isn't complete
         * or is invalid.
         * @throw Methan::Exception If the configuration for creating the context is invalid
         * 
         * @return Context the newly created Context
         */
        METHAN_API Context build();

    private:
        std::vector<Methan::Varient> m_sinks;
        EComputerType m_computerType;
    };

    /**
     * @brief Free a given context and the sub-object allocated within it.
     * @warning This methods ain't thread-safe and must be called after all working processes
     * are terminated. Calling it during an execution will result in undefined behavior
     * 
     * @param context The context that will be destroyed 
     */
    METHAN_API void free(Context context);

}
