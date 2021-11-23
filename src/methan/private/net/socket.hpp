#pragma once

#include <methan/core/except.hpp>
#include <methan/core/context.hpp>
#include <methan/core/contextuable.hpp>
#include <methan/core/net.hpp>
#include <methan/utility/expected.hpp>
#include <methan/utility/varient.hpp>
#include <methan/utility/expected.hpp>

namespace Methan {

    enum class EDNSQueryErrorType : uint8_t
    {
        HostNotFound,
        NoRecordAvailable,
        TryAgain
    };

    enum class ESocketErrorType : uint8_t
    {
        Ok = 0,
        SocketAlreadyConnected,
        NotReachable,
        ConnectionAborted,
    };

    enum class ETransportLayerType : uint8_t
    {
        Tcp,
        Udp
    };

    METHAN_API void init_network(Context context);
    METHAN_API void clean_network(Context context);

    METHAN_API tl::expected<ResolvedHost, EDNSQueryErrorType> queryDNS(Context context, DomainName domainName, IpType preferred);    

    class Socket : public Contextuable
    {
    public:
        METHAN_API Socket(Context context, ETransportLayerType transportType, IpType family);
        METHAN_API ~Socket();

        METHAN_API ESocketErrorType connect(ResolvedHost host, uint16_t port);
        METHAN_API ESocketErrorType send(std::string& data);
        METHAN_API ESocketErrorType receive(std::string* buffer);

        METHAN_API ESocketErrorType bind(uint16_t port);

        inline ETransportLayerType transport_layer_type() const noexcept
        {
            return m_transportType;
        }

    private:
        METHAN_API void __closesocket();

        ETransportLayerType m_transportType;
        IpType m_family;
        Methan::Varient m_socket;
        uint8_t m_flag;
        std::array<uint8_t, METHAN_BUFFER_SIZE> m_buffer;
    };

}
