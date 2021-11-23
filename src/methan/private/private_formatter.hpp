#pragma once

#include <string>

#include <methan/core/except.hpp>
#include <methan/utility/uuid.hpp>
#include <methan/core/net.hpp>
#include <spdlog/fmt/bundled/format.h>

template<>
struct fmt::formatter<Methan::Uuid> : fmt::formatter<std::string>
{
    template<typename FormatContext>
    auto format(Methan::Uuid uuid, FormatContext& ctx)
    {
        return formatter<std::string>::format(uuid.to_string(), ctx);
    }
};

template<>
struct fmt::formatter<Methan::ResolvedHost> : fmt::formatter<std::string>
{
    template<typename FormatContext>
    auto format(Methan::ResolvedHost host, FormatContext& ctx)
    {
        return formatter<std::string>::format(Methan::to_string(host), ctx);
    }
};

