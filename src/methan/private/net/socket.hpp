#pragma once

#include <methan/core/except.hpp>
#include <methan/core/context.hpp>
#include <methan/core/contextuable.hpp>
#include <methan/core/net.hpp>
#include <methan/utility/expected.hpp>

namespace Methan {

    enum class EDNSQueryErrorType : uint8_t
    {
        HostNotFound,
        NoRecordAvailable,
        TryAgain
    };

    METHAN_API void init_network(Context context);
    METHAN_API void clean_network(Context context);

    METHAN_API tl::expected<ResolvedHost, EDNSQueryErrorType> queryDNS(Context context, DomainName domainName, IpType preferred);    

}
