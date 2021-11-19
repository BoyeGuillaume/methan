#pragma once

#include <cstring>
#include <type_traits>

#include <methan/core/except.hpp>
#include <methan/utility/assertion.hpp>
#include <methan/utility/crc32.hpp>

#ifdef METHAN_DEBUG
#define METHAN_IDENTIFIER(name)                                   Methan::StringIdentifier(name)
#else
#define METHAN_IDENTIFIER(name)                                   std::integral_constant<Methan::StringIdentifier, Methan::__private__::crc32(name, sizeof(name) - 1)>::value
#endif

namespace Methan {

#ifdef METHAN_DEBUG
    class StringIdentifier
    {
    public:
        inline StringIdentifier(const char* name) noexcept
        : m_name(name),
        m_hash(__private__::crc32(name, strlen(name)))
        {}
        
        inline friend bool operator==(const StringIdentifier& lhs, const StringIdentifier& rhs)
        {
            if(strcmp(lhs.m_name, rhs.m_name) == 0)
            {
                METHAN_ASSERT(lhs.m_hash == rhs.m_hash, Methan::ExceptionType::HashCollision, "crc32 hash collision between \"" + std::string(lhs.m_name) + "\" and \"" + std::string(rhs.m_name) + "\"");
                return true;
            }

            METHAN_ASSERT(lhs.m_hash != rhs.m_hash, Methan::ExceptionType::HashCollision, "crc32 hash collision between \"" + std::string(lhs.m_name) + "\" and \"" + std::string(rhs.m_name) + "\"");
            return false;
        }

        inline friend bool operator!=(const StringIdentifier& lhs, const StringIdentifier& rhs)
        {
            return !(lhs == rhs);
        } 

    private:
        const char* m_name;
        uint32_t m_hash;
    };
#else
    typedef uint32_t StringIdentifier;
#endif

}