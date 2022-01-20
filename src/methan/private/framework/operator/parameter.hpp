#pragma once

#include <variant>

#include <methan/core/except.hpp>
#include <methan/core/dtype.hpp>
#include <methan/utility/varient.hpp>


namespace Methan {

    class Parameter
    {
    public:
        template<typename T>
        inline Parameter(T t) : m_value(t)
        { }

        template<typename T>
        inline T get(T t) const
        {
            return std::get<T>(m_value);
        }

        inline bool operator==(const Parameter& other) const
        {
            return m_value == other.m_value;
        }

        inline bool operator!=(const Parameter& other) const
        {
            return !(*this == other);
        }

        template<typename T>
        inline T operator()() const
        {
            return std::get<T>(m_value);
        }

    private:
        std::variant<uint8_t, uint16_t, uint32_t, uint64_t, int8_t, int16_t, int32_t, int64_t, float, double> m_value;
    };

}
