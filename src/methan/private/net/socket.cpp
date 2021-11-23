#include <methan/private/net/socket.hpp>
#include <methan/private/private_context.hpp>
#include <methan/core/details/platform.hpp>
#include <methan/utility/assertion.hpp>
#include <methan/private/private_constant.hpp>
#include <methan/private/private_visit_variant_helper.hpp>
#include <methan/private/private_formatter.hpp>
#include <mutex>
#include <variant>
#include <stdlib.h>

// https://github.com/microsoft/Windows-classic-samples/tree/main/Samples/Win7Samples/netds/winsock

#define METHAN_SOCKET_FLAG_ALIVE               1
#define METHAN_SOCKET_FLAG_CONNECT             2


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

METHAN_API std::string Methan::to_string(const ResolvedHost& host)
{
    std::string output;
    std::visit(overloaded {
        [&output](const IpV4& ipv4)
        {
            for(size_t i = 0; i < 4; ++i)
            {
                output += std::to_string((int) ipv4[i]);
                if(i != 3) output += ".";
            }
        },
        [&output](const IpV6& ipv6)
        {
            static constexpr const char* hex_digit = "0123456789abcdef";
            for(size_t i = 0; i < 16; ++i)
            {
                output += hex_digit[ipv6[i] & 0xF];
                output += hex_digit[(ipv6[i] >> 4) & 0xF];
                if(i != 15) output += ":";   
            }
        }
    }, host);
    return output;
}

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
    queryDNS_raw(Methan::Context context, Methan::DomainName domainName, Methan::IpType preferred)
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
            return tl::make_unexpected(Methan::EDNSQueryErrorType::HostNotFound);
        }
        else if(r == WSANO_DATA)
        {
            METHAN_LOG_ERROR(context->logger, "Methan::queryDNS(Context, DomainName) failed with the given host \"{}\", ErrCode: NoData (The requested name is valid, but no data of the requested type was found)", domainName);
            return tl::make_unexpected(Methan::EDNSQueryErrorType::NoRecordAvailable);
        }
        else if(r == WSATRY_AGAIN)
        {
            METHAN_LOG_WARNING(context->logger, "Methan::queryDNS(Context, DomainName) failed to resolve the given host \"{}\", ErrCode: TryAgain", domainName);
            return tl::make_unexpected(Methan::EDNSQueryErrorType::TryAgain);
        }
        else
        {
            METHAN_LOG_ERROR(context->logger, "Methan::queryDNS(Context, DomainName) failed with unknown error for host {}, ErrCode: {}", domainName, r);
            METHAN_THROW_EXCEPTION("Methan::queryDNS(Context) failure with unkown error code", Methan::ExceptionType::Unknown);
        }
    }

    Methan::IpType current = Methan::IpType::None;
    Methan::ResolvedHost resolvedHost;

    for(auto ptr = result; ptr != NULL; ptr = ptr->ai_next)
    {
        switch(ptr->ai_family)
        {
        case AF_INET:
            if(current == Methan::IpType::None || preferred == Methan::IpType::Ipv4) {
                current = Methan::IpType::Ipv4;
                sockaddr_ipv4 = (struct sockaddr_in *) ptr->ai_addr;
                Methan::IpV4 ipv4;
                *(uint32_t*) ipv4.data() = sockaddr_ipv4->sin_addr.s_addr;
                resolvedHost = ipv4;
            }
            break;
        case AF_INET6:
            if(current == Methan::IpType::Ipv6 || preferred == Methan::IpType::Ipv6) {
                current = Methan::IpType::Ipv6;
                sockaddr_ipv6 = (struct sockaddr_in6 *) ptr->ai_addr;
                Methan::IpV6 ipv6;
                memcpy(ipv6.data(), sockaddr_ipv6->sin6_addr.u.Byte, ipv6.size());
                resolvedHost = ipv6;
            }
            break;
        }
    }

    freeaddrinfo(result); // Don't forget to free

    if(current == Methan::IpType::None) {
        METHAN_LOG_ERROR(context->logger, "Methan::queryDNS(Context, DomainName) failed resolving the host \"{}\" has no record holds informations about the Ipv4 / Ipv6");
        return tl::make_unexpected(Methan::EDNSQueryErrorType::NoRecordAvailable);
    }

    return resolvedHost;

#elif defined(METHAN_OS_UNIX_LIKE)
    std::lock_guard guard(context->__dns_requests_m);
    struct hostent * result = NULL;
    if(preferred != Methan::IpType::None) {
        result = gethostbyname2(domainName.c_str(), (preferred == Methan::IpType::Ipv4) ? AF_INET : AF_INET6);

        if(result == NULL && (h_errno == NO_ADDRESS || h_errno == NO_DATA)) {
            result = gethostbyname(domainName.c_str());
        }
    }

    if (result == NULL)
    {
        int err = h_errno;
        if(err == HOST_NOT_FOUND)
        {
            METHAN_LOG_ERROR(context->logger, "Methan::queryDNS(Context, DomainName) failed with the given host \"{}\", ErrCode: HostNotFound", domainName);
            return tl::make_unexpected(Methan::EDNSQueryErrorType::HostNotFound);
        }
        else if(err == NO_ADDRESS || err == NO_DATA)
        {
            METHAN_LOG_ERROR(context->logger, "Methan::queryDNS(Context, DomainName) failed with the given host \"{}\", ErrCode: NoData (The requested name is valid, but no data of the requested type was found)", domainName);
            return tl::make_unexpected(Methan::EDNSQueryErrorType::NoRecordAvailable);
        }
        else if(err == TRY_AGAIN)
        {
            METHAN_LOG_WARNING(context->logger, "Methan::queryDNS(Context, DomainName) failed to resolve the given host \"{}\", ErrCode: TryAgain", domainName);
            return tl::make_unexpected(Methan::EDNSQueryErrorType::TryAgain);
        }
        else
        {
            METHAN_LOG_ERROR(context->logger, "Methan::queryDNS(Context, DomainName) failed with unknown error for host {}, ErrCode: {}", domainName, err);
            METHAN_THROW_EXCEPTION("Methan::queryDNS(Context) failure with unkown error code", Methan::ExceptionType::Unknown);
        }
    }

    Methan::ResolvedHost resolvedHost;
    if(result->h_addrtype == AF_INET)
    {
        Methan::IpV4 ipv4;
        memcpy(ipv4.data(), result->h_addr_list[0], ipv4.size());
        resolvedHost = ipv4;
    }
    else if(result->h_addrtype == AF_INET6)
    {
        Methan::IpV6 ipv6;
        memcpy(ipv6.data(), result->h_addr_list[0], ipv6.size());
        resolvedHost = ipv6;
    }
    else
    {
        METHAN_LOG_ERROR(context->logger, "Methan::queryDNS(Context, DomainName) failed resolving the host \"{}\" has no record holds informations about the Ipv4 / Ipv6");
        return tl::make_unexpected(Methan::EDNSQueryErrorType::NoRecordAvailable);
    }

    return resolvedHost;
#endif
}

METHAN_API tl::expected<Methan::ResolvedHost, Methan::EDNSQueryErrorType>
    Methan::queryDNS(Methan::Context context, Methan::DomainName domainName, Methan::IpType preferred)
{
    tl::expected<Methan::ResolvedHost, Methan::EDNSQueryErrorType> result;

    for(size_t i = 0; i <= METHAN_MAX_DNS_RETRY + 1; ++i)
    {
        result = queryDNS_raw(context, domainName, preferred);
        if(!result.has_value() || (result.error() != Methan::EDNSQueryErrorType::TryAgain)) break;
    }

    return result;
}

METHAN_API Methan::Socket::Socket(Context context, ETransportLayerType transportType, IpType family)
: Contextuable(context),
  m_transportType(transportType),
  m_family(family),
  m_socket(nullptr),
  m_flag(0x0)
{
    METHAN_ASSERT_NON_NULL(context);
    
    {
        std::lock_guard guard(context->__init_m);
        if((context->cflag & METHAN_COMPONENT_NETWORK) == 0)
        {
            METHAN_LOG_ERROR(context->logger, "Socket::Socket() failed as the network component was not initialized (or already dispose of)");
            METHAN_THROW_EXCEPTION("Methan::Context() failure due to non-initialized component network", Methan::ExceptionType::IllegalState);
        }
    }

#ifdef METHAN_OS_WINDOWS
    SOCKET socket = INVALID_SOCKET;

    int af = AF_UNSPEC;
    if(m_family == IpType::Ipv6) af = AF_INET6;
    else if(m_family == IpType::Ipv4) af = AF_INET;

    int proto = 0;
    int type = 0;
    
    if(m_transportType == ETransportLayerType::Tcp) 
    {
        proto = IPPROTO_TCP;
        type = SOCK_STREAM;
    }
    else if(m_transportType == ETransportLayerType::Udp)
    {
        proto = IPPROTO_UDP;
        type = SOCK_DGRAM;
    }

    socket = WSASocketW(af, type, proto, NULL, 0, 0); // WSA_FLAG_OVERLAPPED
    if(socket == INVALID_SOCKET)
    {
        int err = WSAGetLastError();
        if(err == WSAENETDOWN)
        {
            METHAN_LOG_ERROR(context->logger, "Socket::Socket() failed as a network subsystem as failed (internal windows error)");
        }
        else if(err == WSAESOCKTNOSUPPORT)
        {
            METHAN_LOG_ERROR(context->logger, "Socket::Socket() failed as the specified family ain't supported.");
        }
        else
        {
            METHAN_LOG_ERROR(context->logger, "Socket::Socket(), WsaGetLastError() return ErrCode: {}", err);
        }

        METHAN_THROW_EXCEPTION("Socket::Socket() failed with ErrCode: " + std::to_string(err), Methan::ExceptionType::Unknown);
    }

    m_socket = socket;
    m_flag |= METHAN_SOCKET_FLAG_ALIVE;
#endif
}

METHAN_API Methan::Socket::~Socket()
{
    if(m_flag & METHAN_SOCKET_FLAG_ALIVE)
    {
        __closesocket();
    }
}

METHAN_API void Methan::Socket::__closesocket()
{
#ifdef METHAN_OS_WINDOWS
    SOCKET socket = m_socket.get<SOCKET>();
    if(closesocket(socket) == SOCKET_ERROR)
    {
        int err = WSAGetLastError();
        if(err == WSAEINPROGRESS)
        {
            METHAN_LOG_ERROR(context()->logger, "Socket::__closesocket() failed to call `closesocket()` as a blocking windows socket is in progress / or still processing callback function", err);
        }
        else 
        {
            METHAN_LOG_ERROR(context()->logger, "Socket::__closesocket() failed to call `closesocket()` on the socket with error : {}", err);
        }
    }
    m_socket = nullptr;
    m_flag &= ~METHAN_SOCKET_FLAG_ALIVE;
#endif
}

METHAN_API Methan::ESocketErrorType Methan::Socket::connect(ResolvedHost host, uint16_t port)
{
    if((m_flag & METHAN_SOCKET_FLAG_ALIVE) == 0x0)
    {
        METHAN_LOG_ERROR(context()->logger, "Socket::connect failed as the socket was not correctly initiazed or has already been disposed of.");
        METHAN_INVALID_STATE;
    }

#ifdef METHAN_DEBUG
    if(m_flag & METHAN_SOCKET_FLAG_CONNECT)
    {
        METHAN_LOG_WARNING(context()->logger, "Socket::connect was called on already connected socket");
        return ESocketErrorType::Ok;
    }

    if(m_transportType == ETransportLayerType::Udp)
    {
        METHAN_LOG_ERROR(context()->logger, "Socket::connect failed as an illegal attempt of connecting an UDP-socket to an target has been made");
        METHAN_INVALID_STATE;
    }
#endif

#ifdef METHAN_OS_WINDOWS
    struct sockaddr * addr;
    std::visit(overloaded {
        [port, &addr](const IpV4& ipv4)
        { // Setup the socket with AF_INET
            struct sockaddr_in * _addr = (sockaddr_in*) malloc(sizeof(sockaddr_in));
            ZeroMemory(_addr, sizeof(sockaddr_in));
            memcpy(&_addr->sin_addr.s_addr, ipv4.data(), ipv4.size());
            _addr->sin_family = AF_INET;
            _addr->sin_port = htons(port);
            addr = (sockaddr*) _addr;
        },
        [port, &addr](const IpV6& ipv6)
        { // Setup the socket with AF_INET6
            struct sockaddr_in6 * _addr = (sockaddr_in6*) malloc(sizeof(sockaddr_in6));
            ZeroMemory(_addr, sizeof(sockaddr_in6));
            memcpy(_addr->sin6_addr.u.Byte, ipv6.data(), ipv6.size());
            _addr->sin6_family = AF_INET6;
            _addr->sin6_port = htons(port);
            addr = (sockaddr*) _addr;
        }
    }, host);

    int result = ::connect(m_socket.get<SOCKET>(), addr, 0);
    ::free(addr);

    if(result != 0)
    {
        int err = WSAGetLastError();
        if(err == WSAEADDRINUSE)
        {
            METHAN_LOG_ERROR(context()->logger, "Socket::connect({}) failed with error \"The socket's local address is already in use and the socket was not marked to allow address reuse with SO_REUSEADDR.\"", host);
            return ESocketErrorType::SocketAlreadyConnected;
        }
        else if(err == WSAEALREADY)
        {
            METHAN_LOG_ERROR(context()->logger, "Socket::connect({}) failed with error \"A nonblocking connect call is in progress on the specified socket.\"", host);
        }
        else if(err == WSAEADDRNOTAVAIL)
        {
            METHAN_LOG_ERROR(context()->logger, "Socket::connect({}) failed with error \"The remote address is not a valid address (such as INADDR_ANY or in6addr_any).\"", host);
            return ESocketErrorType::NotReachable;
        }
        else if(err == WSAECONNREFUSED)
        {
            METHAN_LOG_WARNING(context()->logger, "Socket::connect({}) failed as the connection was refused");
            return ESocketErrorType::ConnectionAborted;
        }
        else if(err == WSAECONNREFUSED)
        {
            METHAN_LOG_ERROR(context()->logger, "Socket::connect({}) failed with error \"The attempt to connect was forcefully rejected.\"", host);
        }
        else if(err == WSAENETUNREACH)
        {
            METHAN_LOG_ERROR(context()->logger, "Socket::connect({}) failed with error \"The network cannot be reached from this host at this time.\"", host);
            return ESocketErrorType::NotReachable;
        }
        else
        {
            METHAN_LOG_ERROR(context()->logger, "Socket::connect({}) failed with ErrCode {}", host, WSAGetLastError());
        }
    }
    m_flag |= METHAN_SOCKET_FLAG_CONNECT;
    return ESocketErrorType::Ok;
#endif
}

METHAN_API Methan::ESocketErrorType Methan::Socket::send(std::string& data)
{
    if(m_socket.is_empty())
    {
        METHAN_LOG_ERROR(context()->logger, "Socket::connect failed as the socket was not correctly initiazed or has already been disposed of.");
        METHAN_INVALID_STATE;
    }

    if((m_flag & METHAN_SOCKET_FLAG_CONNECT) == 0)
    {
        METHAN_LOG_ERROR(context()->logger, "Socket::connect failed as the socket was not connected to any host");
        METHAN_INVALID_STATE;
    }

#ifdef METHAN_OS_WINDOWS
    DWORD size_write = 0;
    WSABUF lpbuffer;
    lpbuffer.buf = data.data();
    lpbuffer.len = (ULONG) data.size();
    int r = WSASend(m_socket.get<SOCKET>(), &lpbuffer, 1, &size_write, 0, NULL, NULL);
    if(r != 0)
    {
        int err = WSAGetLastError();
        METHAN_LOG_ERROR(context()->logger, "Socket::send failed with ErrCode: {}", err);
        if(err == WSAECONNABORTED || err == WSAECONNRESET)
        {
            return ESocketErrorType::ConnectionAborted;
        }

        METHAN_INVALID_STATE;
    }
    return ESocketErrorType::Ok;
#endif
}

METHAN_API Methan::ESocketErrorType Methan::Socket::receive(std::string* buffer)
{
    if(m_socket.is_empty())
    {
        METHAN_LOG_ERROR(context()->logger, "Socket::receive failed as the socket was not correctly initiazed or has already been disposed of.");
        METHAN_INVALID_STATE;
    }

    if((m_flag & METHAN_SOCKET_FLAG_CONNECT) == 0)
    {
        METHAN_LOG_ERROR(context()->logger, "Socket::receive failed as the socket was not connected to any host");
        METHAN_INVALID_STATE;
    }

#ifdef METHAN_OS_WINDOWS
    WSABUF lpbuffer;
    lpbuffer.buf = (CHAR*) m_buffer.data();
    lpbuffer.len = (ULONG) m_buffer.size();
    DWORD lpNumberOfBytesRecvd;
    DWORD flag = 0;
    if(WSARecv(m_socket.get<SOCKET>(), &lpbuffer, 1, &lpNumberOfBytesRecvd, &flag, NULL, NULL) != 0)
    {
        int err = WSAGetLastError();
        METHAN_LOG_ERROR(context()->logger, "Socket::receive failed with ErrCode: {}", err);
        if(err == WSAECONNABORTED || err == WSAECONNRESET)
        {
            return ESocketErrorType::ConnectionAborted;
        }

        METHAN_INVALID_STATE;
    }

    buffer->append((const char*) m_buffer.data(), (size_t) lpNumberOfBytesRecvd);
    return ESocketErrorType::Ok;
#endif
}

METHAN_API Methan::ESocketErrorType Methan::Socket::bind(uint16_t port)
{
    if(m_socket.is_empty())
    {
        METHAN_LOG_ERROR(context()->logger, "Socket::bind failed as the socket was not correctly initiazed or has already been disposed of.");
        METHAN_INVALID_STATE;
    }

    if(m_flag & METHAN_SOCKET_FLAG_CONNECT)
    {
        METHAN_LOG_ERROR(context()->logger, "Socket::bind failed as the socket was already connected to some hosts");
        METHAN_INVALID_STATE;
    }

#ifdef METHAN_OS_WINDOWS
    
#endif
}
