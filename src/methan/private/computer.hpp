#pragma once

#include <methan/core/except.hpp>
#include <methan/core/serializable.hpp>
#include <methan/utility/uuid.hpp>

namespace Methan {


    enum class EComputerType : uint8_t
    {
        Observer,
        Master,
        Slave
    };

    struct ComputerIdentity
    {
        std::string hostname;
        uint16_t port; /** If the computer is configured in network, this computer will listen for incomming connection on this port **/
    
        METHAN_SERDE_GENERATOR(ComputerIdentity, hostname, port);
    };

    struct ComputerDescriptor
    {
        Uuid uuid;
        ComputerIdentity identity;
        EComputerType type;

        METHAN_SERDE_GENERATOR(ComputerDescriptor, uuid, identity, type);
    };

}
