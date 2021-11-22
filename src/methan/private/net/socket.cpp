#include <methan/private/net/socket.hpp>
#include <methan/private/private_context.hpp>
#include <methan/core/details/platform.hpp>
#include <methan/utility/assertion.hpp>
#include <mutex>
#include <variant>
#include <stdlib.h>

#if defined(METHAN_OS_WINDOWS)
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")
#pragma comment(lib, "AdvApi32.lib")
#endif

#if defined(METHAN_OS_UNIX_LIKE)
#include <netdb.h>
#include <sys/socket.h>
#endif

METHAN_API void Methan::init_network(Context context)
{
    METHAN_ASSERT_NON_NULL(context);

    std::lock_guard guard(context->__init_m);
    if(context->cflag & METHAN_COMPONENT_NETWORK)
    {
        METHAN_LOG_WARNING(context->logger, "Methan::init_netwtork(Context) has already been called for that specific context.");
        return;
    }

#ifdef METHAN_OS_WINDOWS
    WSADATA wsaData;
    int r = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if(r != 0)
    {
        METHAN_LOG_ERROR(context->logger, "Methan::init_network(Context) failed to call `WSAStartup()` with error code {}", r);
        METHAN_THROW_EXCEPTION("Methan::init_network(Context) failure", Methan::ExceptionType::Unknown);
    }
#endif

    METHAN_LOG_DEBUG(context->logger, "Methan::init_network(Context) successfully executed");
    context->cflag |= METHAN_COMPONENT_NETWORK;
}

METHAN_API void Methan::clean_network(Context context)
{
    METHAN_ASSERT_NON_NULL(context);

    std::lock_guard guard(context->__init_m);
    if((context->cflag & METHAN_COMPONENT_NETWORK) == 0)
    {
        METHAN_LOG_ERROR(context->logger, "Methan::clean_network(Context) has been called in an context where the network has not been yet initialized (or already cleaned).");
        return;
    }

#ifdef METHAN_OS_WINDOWS
    int r = WSACleanup();
    if(r != 0)
    {
        METHAN_LOG_ERROR(context->logger, "Methan::clean_network(Context) failed to call `WSACleanup()` with error code {}", r);
        METHAN_THROW_EXCEPTION("Methan::clean_network(Context) failure", Methan::ExceptionType::Unknown);
    }
#endif

    METHAN_LOG_DEBUG(context->logger, "Methan::clean_network(Context) successfully executed");
    context->cflag &= ~METHAN_COMPONENT_NETWORK;
}

METHAN_API tl::expected<Methan::ResolvedHost, Methan::EDNSQueryErrorType>
    Methan::queryDNS(Context context, DomainName domainName, IpType preferred)
{
    METHAN_ASSERT_NON_NULL(context);

    {
        std::lock_guard guard(context->__init_m);
        if((context->cflag & METHAN_COMPONENT_NETWORK) == 0)
        {
            METHAN_LOG_ERROR(context->logger, "Methan::queryDNS(Context, DomainName) failed as the network component has not been yet initialized (or has been cleaned)");
            METHAN_THROW_EXCEPTION("Methan::queryDNS(Context) failure", Methan::ExceptionType::IllegalState);
        }
    }

#if defined(METHAN_OS_WINDOWS)
    struct addrinfo *result = NULL;
    struct addrinfo *ptr = NULL;
    struct addrinfo hints;
    struct sockaddr_in *sockaddr_ipv4;
    struct sockaddr_in6 *sockaddr_ipv6;

    ZeroMemory(&hints, sizeof( hints ));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    int r = getaddrinfo(domainName.c_str(), NULL, &hints, &result);
    if(r != 0)
    {
        if(r == WSA_NOT_ENOUGH_MEMORY)
        {
            METHAN_LOG_CRITICAL(context->logger, "Methan::queryDNS(Context, DomainName) failed as there is not enought memory (WSA_NOT_ENOUGH_MEMORY)");
            METHAN_THROW_EXCEPTION("Methan::queryDNS(Context) failure", Methan::ExceptionType::NotEnoughMemory);
        }
        else if(r == WSAHOST_NOT_FOUND)
        {
            METHAN_LOG_ERROR(context->logger, "Methan::queryDNS(Context, DomainName) failed with the given host \"{}\", ErrCode: HostNotFound", domainName);
            return tl::make_unexpected(EDNSQueryErrorType::HostNotFound);
        }
        else if(r == WSANO_DATA)
        {
            METHAN_LOG_ERROR(context->logger, "Methan::queryDNS(Context, DomainName) failed with the given host \"{}\", ErrCode: NoData (The requested name is valid, but no data of the requested type was found)", domainName);
            return tl::make_unexpected(EDNSQueryErrorType::NoRecordAvailable);
        }
        else if(r == WSATRY_AGAIN)
        {
            METHAN_LOG_WARNING(context->logger, "Methan::queryDNS(Context, DomainName) failed to resolve the given host \"{}\", ErrCode: TryAgain", domainName);
            return tl::make_unexpected(EDNSQueryErrorType::TryAgain);
        }
        else
        {
            METHAN_LOG_ERROR(context->logger, "Methan::queryDNS(Context, DomainName) failed with unknown error for host {}, ErrCode: {}", domainName, r);
            METHAN_THROW_EXCEPTION("Methan::queryDNS(Context) failure with unkown error code", Methan::ExceptionType::Unknown);
        }
    }

    IpType current = IpType::None;
    ResolvedHost resolvedHost;

    for(auto ptr = result; ptr != NULL; ptr = ptr->ai_next)
    {
        switch(ptr->ai_family)
        {
        case AF_INET:
            if(current == IpType::None || preferred == IpType::Ipv4) {
                current = IpType::Ipv4;
                sockaddr_ipv4 = (struct sockaddr_in *) ptr->ai_addr;
                IpV4 ipv4;
                *(uint32_t*) ipv4.data() = sockaddr_ipv4->sin_addr.s_addr;
                resolvedHost = ipv4;
            }
            break;
        case AF_INET6:
            if(current == IpType::Ipv6 || preferred == IpType::Ipv6) {
                current = IpType::Ipv6;
                sockaddr_ipv6 = (struct sockaddr_in6 *) ptr->ai_addr;
                IpV6 ipv6;
                memcpy(ipv6.data(), sockaddr_ipv6->sin6_addr.u.Byte, ipv6.size());
                resolvedHost = ipv6;
            }
            break;
        }
    }

    freeaddrinfo(result); // Don't forget to free

    if(current == IpType::None) {
        METHAN_LOG_ERROR(context->logger, "Methan::queryDNS(Context, DomainName) failed resolving the host \"{}\" has no record holds informations about the Ipv4 / Ipv6");
        return tl::make_unexpected(EDNSQueryErrorType::NoRecordAvailable);
    }

    return resolvedHost;

#elif defined(METHAN_OS_UNIX_LIKE)
    std::lock_guard guard(context->__dns_requests_m);
    struct hostent * result = gethostbyname(domainName.c_str());
    
    if (result == NULL)
    {
        int err = h_errno;
        if(err == HOST_NOT_FOUND)
        {
            METHAN_LOG_ERROR(context->logger, "Methan::queryDNS(Context, DomainName) failed with the given host \"{}\", ErrCode: HostNotFound", domainName);
            return tl::make_unexpected(EDNSQueryErrorType::HostNotFound);
        }
        else if(err == NO_ADDRESS || err == NO_DATA)
        {
            METHAN_LOG_ERROR(context->logger, "Methan::queryDNS(Context, DomainName) failed with the given host \"{}\", ErrCode: NoData (The requested name is valid, but no data of the requested type was found)", domainName);
            return tl::make_unexpected(EDNSQueryErrorType::NoRecordAvailable);
        }
        else if(err == TRY_AGAIN)
        {
            METHAN_LOG_WARNING(context->logger, "Methan::queryDNS(Context, DomainName) failed to resolve the given host \"{}\", ErrCode: TryAgain", domainName);
            return tl::make_unexpected(EDNSQueryErrorType::TryAgain);
        }
        else
        {
            METHAN_LOG_ERROR(context->logger, "Methan::queryDNS(Context, DomainName) failed with unknown error for host {}, ErrCode: {}", domainName, err);
            METHAN_THROW_EXCEPTION("Methan::queryDNS(Context) failure with unkown error code", Methan::ExceptionType::Unknown);
        }
    }

    ResolvedHost resolvedHost;
    if(result->h_addrtype == AF_INET)
    {
        IpV4 ipv4;
        memcpy(ipv4.data(), result->h_addr_list[0], ipv4.size());
        resolvedHost = ipv4;
    }
    else if(result->h_addrtype == AF_INET6)
    {
        IpV6 ipv6;
        memcpy(ipv6.data(), result->h_addr_list[0], ipv6.size());
        resolvedHost = ipv6;
    }
    else
    {
        METHAN_LOG_ERROR(context->logger, "Methan::queryDNS(Context, DomainName) failed resolving the host \"{}\" has no record holds informations about the Ipv4 / Ipv6");
        return tl::make_unexpected(EDNSQueryErrorType::NoRecordAvailable);
    }

    return resolvedHost;

#endif
}

