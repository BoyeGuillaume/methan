#pragma once

#include <string>

#include <methan/core/except.hpp>
#include <methan/utility/uuid.hpp>
#include <methan/utility/data_size.hpp>
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
struct fmt::formatter<Methan::DataSize> : fmt::formatter<std::string>
{
    template<typename FormatContext>
    auto format(Methan::DataSize size, FormatContext& ctx)
    {
        return formatter<std::string>::format(Methan::to_string(size), ctx);
    }
};

