#pragma once

#include <memory>
#include <mutex>
#include <atomic>
#include <vector>
#include <unordered_map>

#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/fmt/bin_to_hex.h>

#include <methan/core/except.hpp>
#include <methan/core/context.hpp>
#include <methan/utility/uuid.hpp>
#include <methan/private/framework/framework.hpp>

#define METHAN_LOG(logger, level, ...)                                                 logger->log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, level, __VA_ARGS__)
#define METHAN_LOG_DEBUG(logger, ...)                                                  METHAN_LOG(logger, spdlog::level::debug, __VA_ARGS__)
#define METHAN_LOG_TRACE(logger, ...)                                                  METHAN_LOG(logger, spdlog::level::trace, __VA_ARGS__)
#define METHAN_LOG_ERROR(logger, ...)                                                  METHAN_LOG(logger, spdlog::level::err, __VA_ARGS__)
#define METHAN_LOG_CRITICAL(logger, ...)                                               METHAN_LOG(logger, spdlog::level::critical, __VA_ARGS__)
#define METHAN_LOG_WARNING(logger, ...)                                                METHAN_LOG(logger, spdlog::level::warn, __VA_ARGS__)
#define METHAN_LOG_INFO(logger, ...)                                                   METHAN_LOG(logger, spdlog::level::info, __VA_ARGS__)

#define METHAN_COMPONENT_LOGGER              (1 << 0)

namespace Methan {

    typedef std::pair<Uuid,Uuid> UuidPair;

}

namespace std {

    template<>
    struct hash<Methan::UuidPair>
    {
        size_t operator()(const Methan::UuidPair& v) const
        {
            return hash<Methan::Uuid>()(v.first) ^ hash<Methan::Uuid>()(v.second);
        }
    };

}

namespace Methan::__private__ {

    struct __Context
    {
        std::shared_ptr<spdlog::details::thread_pool> logger_thread_pool;
        std::shared_ptr<spdlog::async_logger> logger;
    
        std::mutex __uuid_factory_m;
        Methan::UuidFactory uuid_factory;

        std::recursive_mutex __system_m;
        
        std::recursive_mutex __init_m;
        uint32_t cflag; // Flag of all component that as been initialized

        std::vector<AbstractDevice*> devices;
        std::vector<AbstractMemory*> memories;
        std::unordered_map<UuidPair, AbstractDataFlowFactory*> flowsFactories;
        
        std::mutex block_mutex;
        std::unordered_map<Uuid, TensorBlock*> owned_blocks;
    };

    /**
     * @brief Generate an UUID given a context
     * 
     * @param context The context wich will be used to create the UUID. Notice that this
     * methods is a block methods
     * @return Uuid The uuid generated
     */
    inline Uuid generate_uuid(Context context)
    {
        std::lock_guard guard(context->__uuid_factory_m);
        return context->uuid_factory();
    }

    /**
     * @brief Convert the Methan::LogLevel to the corresponding stdlog::level
     * 
     * @param level the Methan::ELogLevel
     * @return spdlog::level::level_enum the corresponding ELogLevel
     */
    inline spdlog::level::level_enum to_spdlog_enum(Methan::ELogLevel level)
    {
        switch(level)
        {
        case Methan::ELogLevel::Off:
            return spdlog::level::off;

        case Methan::ELogLevel::Critical:
            return spdlog::level::critical;

        case Methan::ELogLevel::Warning:
            return spdlog::level::warn;

        case Methan::ELogLevel::Error:
            return spdlog::level::err;
        
        case Methan::ELogLevel::Info:
            return spdlog::level::info;

        case Methan::ELogLevel::Debug:
            return spdlog::level::debug;
        };

        return spdlog::level::debug;
    }

}
