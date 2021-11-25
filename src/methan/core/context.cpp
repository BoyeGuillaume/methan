#include <limits>
#include <memory>
#include <algorithm>

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>

#include <methan/core/context.hpp>
#include <methan/private/private_context.hpp>
#include <methan/private/framework/cpu.hpp>
#include <methan/private/framework/memory/heap.hpp>


METHAN_API Methan::ContextBuilder::ContextBuilder()
: m_cpuCore(0),
  m_heapMemoryLimits(std::numeric_limits<DataSize>::max())
{
    
}

METHAN_API Methan::ContextBuilder::~ContextBuilder()
{
    m_sinks.clear();
}

METHAN_API Methan::ContextBuilder& Methan::ContextBuilder::add_logger_stdout(ELogLevel logLevel, bool supportColor)
{
    spdlog::sink_ptr ptr = nullptr;
    if(supportColor) ptr = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    else ptr = std::make_shared<spdlog::sinks::stdout_sink_mt>();
    ptr->set_level(__private__::to_spdlog_enum(logLevel));
    m_sinks.push_back(Methan::Varient(ptr));
    return *this;
}

METHAN_API Methan::ContextBuilder& Methan::ContextBuilder::add_logger_file(const std::string& filename, ELogLevel level)
{
    spdlog::sink_ptr ptr = std::make_shared<spdlog::sinks::basic_file_sink_mt>(filename);
    ptr->set_level(__private__::to_spdlog_enum(level));
    m_sinks.push_back(ptr);
    return *this;
}

METHAN_API Methan::ContextBuilder& Methan::ContextBuilder::add_logger_rotating_file(const std::string& filename, ELogLevel level, DataSize maxSize, size_t maxFiles)
{
    spdlog::sink_ptr ptr = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(filename, (size_t) maxSize, maxFiles);
    ptr->set_level(__private__::to_spdlog_enum(level));
    m_sinks.push_back(ptr);
    return *this;
}

METHAN_API Methan::ContextBuilder& Methan::ContextBuilder::register_cpu_as_candidate(uint8_t availableCpuCore)
{
    m_cpuCore = availableCpuCore;
    return *this;
}

METHAN_API Methan::ContextBuilder& Methan::ContextBuilder::set_heap_memory_limits(DataSize heapMemoryLimits)
{
    m_heapMemoryLimits = heapMemoryLimits;
    return *this;
}

METHAN_API Methan::Context Methan::ContextBuilder::build()
{
    // Retrieve a list of all the sinks composing the logger
    std::vector<spdlog::sink_ptr> sinks(m_sinks.size(), nullptr);
    std::transform(m_sinks.begin(), m_sinks.end(), sinks.begin(), [](const Methan::Varient& varient) {
        return varient.get<spdlog::sink_ptr>();
    });

    // Create the async logger
    Context context = new __private__::__Context();
    context->cflag = 0x0;
    context->logger_thread_pool = std::make_shared<spdlog::details::thread_pool>(4096, 1);
    context->logger = std::make_shared<spdlog::async_logger>("console", sinks.begin(), sinks.end(), context->logger_thread_pool, spdlog::async_overflow_policy::block);
    context->logger->set_pattern("[%Y-%d-%m %X.%e%z] [%l] [tid=%t] (%s:%#) %v ", spdlog::pattern_time_type::local);
    context->logger->set_level(spdlog::level::debug);

    // Log the context creation
    METHAN_LOG_INFO(context->logger, "Initialisation of the context object at {}", spdlog::fmt_lib::ptr(context));
    context->cflag |= METHAN_COMPONENT_LOGGER;

    // register CPU if require
    if(m_cpuCore != 0) new Cpu(context, m_cpuCore);

    // Register the Heap
    new Heap(context, m_heapMemoryLimits);

    return context;
}

METHAN_API void Methan::free(Methan::Context context)
{
    for(int i = (int) context->devices.size() - 1; i >= 0; --i)
    {
        delete context->devices[i];
    }

    for(int i = (int) context->memories.size() - 1; i >= 0; --i)
    {
        delete context->memories[i];
    }

    METHAN_LOG_INFO(context->logger, "Cleaning and deleting the context object at {}", spdlog::fmt_lib::ptr(context));
    delete context;
}
