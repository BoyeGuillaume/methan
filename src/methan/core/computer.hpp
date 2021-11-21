#pragma once

#include <string>

#include <methan/core/except.hpp>
#include <methan/core/contextuable.hpp>
#include <methan/utility/uuid.hpp>

namespace Methan {


    class Computer : public Contextuable
    {
    public:
        METHAN_API Computer(Context context, std::string hostname, std::string username, const Uuid& uuid, EComputerType computerType);

        inline const std::string& hostname() const {
            return m_hostname;
        }

        inline const std::string& username() const {
            return m_username;
        }

        inline EComputerType computer_type() const {
            return m_computerType;
        }

        inline const Uuid& uuid() const {
            return m_uuid;
        }

    private:
        Uuid m_uuid;
        std::string m_hostname;
        std::string m_username;
        EComputerType m_computerType;
    };

}

