#pragma once

#include <string>
#include <vector>
#include <ostream>
#include <istream>
#include <array>
#include <type_traits>

#include <methan/core/except.hpp>
#include <methan/utility/endianness.hpp>
#include <methan/utility/foreach.hpp>


#define __METHAN_SERIALIZABLE_HELPER_D__(Type, field)                                               Methan::Serde::deserialize(input, u.field);
#define __METHAN_SERIALIZABLE_HELPER_S__(Type, field)                                               Methan::Serde::serialize(output, u.field);

#define METHAN_SERDE_GENERATOR(Type, ...)                                                           \
    friend struct Methan::__private__::__serde<Type>;                                               \
    inline friend void __p_serde__deserialize(std::istream& input, Type& u) {                       \
        METHAN_EXPAND(METHAN_FOR_EACH_P1(__METHAN_SERIALIZABLE_HELPER_D__, Type, __VA_ARGS__))      \
    }                                                                                               \
    inline friend void __p_serde__serialize(std::ostream& output, const Type& u) {                  \
        METHAN_EXPAND(METHAN_FOR_EACH_P1(__METHAN_SERIALIZABLE_HELPER_S__, Type, __VA_ARGS__))      \
    }

namespace Methan::__private__ {

    template<typename T, typename Enabler = void>
    struct __serde
    {
        constexpr __serde() {}

        inline void deserialize(std::istream& input, T& u)
        {
            __p_serde__deserialize(input, u);
        }

        inline void serialize(std::ostream& output, const T& u)
        {
            __p_serde__serialize(output, u);
        }
    };

}

namespace Methan {

    struct Serde
    {
        template<typename T>
        static inline void deserialize(std::istream& input, T& u)
        {
            __private__::__serde<T>().deserialize(input, u);
        }

        template<typename T>
        static inline void serialize(std::ostream& output, const T& u)
        {
            __private__::__serde<T>().serialize(output, u);
        }

        template<typename T, typename = typename std::enable_if<std::is_constructible<T>::value>::type>
        static inline T deserialize(std::istream& input)
        {
            T _u;
            deserialize(input, _u);
            return std::move(_u);
        }
    };

}

#include <methan/core/details/serializable.inl>
