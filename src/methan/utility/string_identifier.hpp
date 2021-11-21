#pragma once

#include <cstring>
#include <type_traits>

#include <methan/core/except.hpp>
#include <methan/core/serializable.hpp>
#include <methan/utility/assertion.hpp>
#include <methan/utility/hash/crc32.hpp>

#ifdef METHAN_DEBUG
#define METHAN_IDENTIFIER(name)                                   Methan::StringIdentifier(name)
#else
#define METHAN_IDENTIFIER(name)                                   std::integral_constant<Methan::StringIdentifier, Methan::__private__::crc32(name, sizeof(name) - 1)>::value
#endif

#if defined(METHAN_COMPILER_GCC)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wliteral-suffix"
#elif defined(METHAN_COMPILER_CLANG)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreserved-user-defined-literal"
#elif defined(METHAN_COMPILER_MSC)
#pragma warning(push)
#pragma warning(disable: 4455)
#endif 

namespace Methan {

#ifdef METHAN_DEBUG
    class StringIdentifier
    {
    public:
        inline StringIdentifier(const char* name)
        : m_name(name),
        m_hash(__private__::crc32(name, strlen(name)))
        {}

        inline StringIdentifier()
        : m_name(""),
        m_hash()
        {}
        
        inline friend bool operator==(const StringIdentifier& lhs, const StringIdentifier& rhs)
        {
            if(lhs.m_name == rhs.m_name)
            {
                METHAN_ASSERT(lhs.m_hash == rhs.m_hash, Methan::ExceptionType::HashCollision, "crc32 hash collision between \"" + lhs.m_name + "\" and \"" + rhs.m_name + "\"");
                return true;
            }

            METHAN_ASSERT(lhs.m_hash != rhs.m_hash, Methan::ExceptionType::HashCollision, "crc32 hash collision between \"" + lhs.m_name + "\" and \"" + rhs.m_name + "\"");
            return false;
        }

        inline friend bool operator!=(const StringIdentifier& lhs, const StringIdentifier& rhs)
        {
            return !(lhs == rhs);
        } 

        inline size_t hash() const
        {
            return std::hash<uint32_t>()(m_hash);
        }

    private:
        std::string m_name;
        uint32_t m_hash;

        METHAN_SERDE_GENERATOR(StringIdentifier, m_name, m_hash);
    };

    inline StringIdentifier operator"" id(const char* data, size_t length) {
        return StringIdentifier(data);
    }
#else
    typedef uint32_t StringIdentifier;

    constexpr StringIdentifier operator"" id(const char* data, size_t length) {
        return Methan::__private__::crc32(data, length);
    }
#endif

}

#ifdef METHAN_DEBUG
namespace std {

    template<>
    struct hash<Methan::StringIdentifier>
    {
        inline size_t operator()(const Methan::StringIdentifier& identifier)
        {
            return identifier.hash();
        }
    };

}
#endif

#if defined(METHAN_COMPILER_GCC) || defined(METHAN_COMPILER_CLANG)
#pragma GCC diagnostic pop
#elif defined(METHAN_COMPILER_MSC)
#pragma warning(pop)
#endif 

