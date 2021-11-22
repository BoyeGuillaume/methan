#pragma once

#include <array>
#include <variant>
#include <string>

#include <methan/core/except.hpp>
#include <methan/core/context.hpp>

namespace Methan {

    enum class IpType {
        Ipv4,
        Ipv6,
        None
    };

    typedef std::array<uint8_t, 4> IpV4;
    typedef std::array<uint8_t, 16> IpV6;
    typedef std::string DomainName;

    typedef std::variant<IpV4, IpV6> ResolvedHost;
    typedef std::variant<IpV4, IpV6, DomainName> Host;

}
