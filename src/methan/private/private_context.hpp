#pragma once

#include <memory>

#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/fmt/bin_to_hex.h>

#include <methan/core/except.hpp>
#include <methan/core/context.hpp>

#define METHAN_LOG(logger, level, ...)                                                 logger->log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, level, __VA_ARGS__)
#define METHAN_LOG_DEBUG(logger, ...)                                                  METHAN_LOG(logger, spdlog::level::debug, __VA_ARGS__)
#define METHAN_LOG_TRACE(logger, ...)                                                  METHAN_LOG(logger, spdlog::level::trace, __VA_ARGS__)
#define METHAN_LOG_ERROR(logger, ...)                                                  METHAN_LOG(logger, spdlog::level::err, __VA_ARGS__)
#define METHAN_LOG_CRITICAL(logger, ...)                                               METHAN_LOG(logger, spdlog::level::critical, __VA_ARGS__)
#define METHAN_LOG_WARNING(logger, ...)                                                METHAN_LOG(logger, spdlog::level::warn, __VA_ARGS__)
#define METHAN_LOG_INFO(logger, ...)                                                   METHAN_LOG(logger, spdlog::level::info, __VA_ARGS__)

namespace Methan::__private__ {

    struct __Context
    {
        std::shared_ptr<spdlog::details::thread_pool> logger_thread_pool;
        std::shared_ptr<spdlog::async_logger> logger;
    };

    /**
     * @brief Convert the Methan::LogLevel to the corresponding stdlog::level
     * 
     * @param level the Methan::LogLevel
     * @return spdlog::level::level_enum the corresponding LogLevel
     */
    inline spdlog::level::level_enum to_spdlog_enum(Methan::LogLevel level)
    {
        switch(level)
        {
        case Methan::LogLevel::Off:
            return spdlog::level::off;

        case Methan::LogLevel::Critical:
            return spdlog::level::critical;

        case Methan::LogLevel::Warning:
            return spdlog::level::warn;

        case Methan::LogLevel::Error:
            return spdlog::level::err;
        
        case Methan::LogLevel::Info:
            return spdlog::level::info;

        case Methan::LogLevel::Debug:
            return spdlog::level::debug;
        };

        return spdlog::level::debug;
    }

}
