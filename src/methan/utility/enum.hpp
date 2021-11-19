#pragma once

#include <type_traits>

#include <methan/core/except.hpp>

/*
    This file is part of Corrade.
    Copyright © 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015, 2016,
                2017, 2018, 2019, 2020, 2021
              Vladimír Vondruš <mosra@centrum.cz>
    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:
    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

#define METHAN_ENUMSET_OPERATORS(class)                                     \
    constexpr bool operator==(class::Type a, class b) {                     \
        return class(a) == b;                                               \
    }                                                                       \
    constexpr bool operator!=(class::Type a, class b) {                     \
        return class(a) != b;                                               \
    }                                                                       \
    constexpr bool operator>=(class::Type a, class b) {                     \
        return class(a) >= b;                                               \
    }                                                                       \
    constexpr bool operator<=(class::Type a, class b) {                     \
        return class(a) <= b;                                               \
    }                                                                       \
    constexpr class operator|(class::Type a, class b) {                     \
        return b | a;                                                       \
    }                                                                       \
    constexpr class operator&(class::Type a, class b) {                     \
        return b & a;                                                       \
    }                                                                       \
    constexpr class operator^(class::Type a, class b) {                     \
        return b ^ a;                                                       \
    }                                                                       \
    constexpr class operator~(class::Type a) {                              \
        return ~class(a);                                                   \
    }



namespace Methan {

    template<class T, typename std::underlying_type<T>::type fullValue = typename std::underlying_type<T>::type(~0)>
    class EnumFlag
    {
        static_assert(std::is_enum<T>::value, "EnumFlag type must be a strongly typed enum");
    
    public:
        typedef T Type;
        typedef typename std::underlying_type<T>::type UnderlyingType;

        constexpr EnumFlag() noexcept : m_value{} {}

        constexpr EnumFlag(T value) noexcept : m_value(static_cast<UnderlyingType>(value)) {}


        constexpr bool operator==(EnumFlag<T, fullValue> other) const {
            return m_value == other.m_value;
        }

        constexpr bool operator!=(EnumFlag<T, fullValue> other) const {
            return !operator==(other);
        }

        constexpr bool operator>=(EnumFlag<T, fullValue> other) const {
            return (*this & other) == other;
        }

        constexpr bool operator<=(EnumFlag<T, fullValue> other) const {
            return (*this & other) == *this;
        }

        constexpr EnumFlag<T, fullValue> operator|(EnumFlag<T, fullValue> other) const {
            return EnumFlag<T, fullValue>(m_value | other.m_value);
        }

        inline EnumFlag<T, fullValue>& operator|=(EnumFlag<T, fullValue> other) {
            m_value |= other.m_value;
            return *this;
        }

        constexpr EnumFlag<T, fullValue> operator&(EnumFlag<T, fullValue> other) const {
            return EnumFlag<T, fullValue>(m_value & other.m_value);
        }

        inline EnumFlag<T, fullValue>& operator&=(EnumFlag<T, fullValue> other) {
            m_value &= other.m_value;
            return *this;
        }
    
        constexpr EnumFlag<T, fullValue> operator^(EnumFlag<T, fullValue> other) const {
            return EnumFlag<T, fullValue>(m_value ^ other.m_value);
        }

        inline EnumFlag<T, fullValue>& operator^=(EnumFlag<T, fullValue> other) {
            m_value ^= other.m_value;
            return *this;
        }

        constexpr EnumFlag<T, fullValue> operator~() const {
            return EnumFlag<T, fullValue>(fullValue & ~m_value);
        }

        constexpr explicit operator bool() const {
            return m_value != 0;
        }

        constexpr explicit operator UnderlyingType() const {
            return m_value;
        }

    private:
        constexpr explicit EnumFlag(UnderlyingType type) noexcept : m_value{type} {}
        UnderlyingType m_value;
    };

}
