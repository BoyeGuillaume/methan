#pragma once
#include <methan/core/serializable.hpp>
#include <type_traits>

namespace Methan::__private__ {

    template<> struct __serde<uint8_t> { inline void deserialize(std::istream& input, uint8_t& u) { uint8_t _u; input.read(reinterpret_cast<char*>(&_u), sizeof(uint8_t)); u = Methan::general_endianness(_u); } inline void serialize(std::ostream& output, const uint8_t& u) { const uint8_t _u = Methan::general_endianness(u); output.write(reinterpret_cast<const char*>(&_u), sizeof(uint8_t)); } };
    template<> struct __serde<uint16_t> { inline void deserialize(std::istream& input, uint16_t& u) { uint16_t _u; input.read(reinterpret_cast<char*>(&_u), sizeof(uint16_t)); u = Methan::general_endianness(_u); } inline void serialize(std::ostream& output, const uint16_t& u) { const uint16_t _u = Methan::general_endianness(u); output.write(reinterpret_cast<const char*>(&_u), sizeof(uint16_t)); } };
    template<> struct __serde<uint32_t> { inline void deserialize(std::istream& input, uint32_t& u) { uint32_t _u; input.read(reinterpret_cast<char*>(&_u), sizeof(uint32_t)); u = Methan::general_endianness(_u); } inline void serialize(std::ostream& output, const uint32_t& u) { const uint32_t _u = Methan::general_endianness(u); output.write(reinterpret_cast<const char*>(&_u), sizeof(uint32_t)); } };
    template<> struct __serde<uint64_t> { inline void deserialize(std::istream& input, uint64_t& u) { uint64_t _u; input.read(reinterpret_cast<char*>(&_u), sizeof(uint64_t)); u = Methan::general_endianness(_u); } inline void serialize(std::ostream& output, const uint64_t& u) { const uint64_t _u = Methan::general_endianness(u); output.write(reinterpret_cast<const char*>(&_u), sizeof(uint64_t)); } };
    template<> struct __serde<int8_t> { inline void deserialize(std::istream& input, int8_t& u) { int8_t _u; input.read(reinterpret_cast<char*>(&_u), sizeof(int8_t)); u = Methan::general_endianness(_u); } inline void serialize(std::ostream& output, const int8_t& u) { const int8_t _u = Methan::general_endianness(u); output.write(reinterpret_cast<const char*>(&_u), sizeof(int8_t)); } };
    template<> struct __serde<int16_t> { inline void deserialize(std::istream& input, int16_t& u) { int16_t _u; input.read(reinterpret_cast<char*>(&_u), sizeof(int16_t)); u = Methan::general_endianness(_u); } inline void serialize(std::ostream& output, const int16_t& u) { const int16_t _u = Methan::general_endianness(u); output.write(reinterpret_cast<const char*>(&_u), sizeof(int16_t)); } };
    template<> struct __serde<int32_t> { inline void deserialize(std::istream& input, int32_t& u) { int32_t _u; input.read(reinterpret_cast<char*>(&_u), sizeof(int32_t)); u = Methan::general_endianness(_u); } inline void serialize(std::ostream& output, const int32_t& u) { const int32_t _u = Methan::general_endianness(u); output.write(reinterpret_cast<const char*>(&_u), sizeof(int32_t)); } };
    template<> struct __serde<int64_t> { inline void deserialize(std::istream& input, int64_t& u) { int64_t _u; input.read(reinterpret_cast<char*>(&_u), sizeof(int64_t)); u = Methan::general_endianness(_u); } inline void serialize(std::ostream& output, const int64_t& u) { const int64_t _u = Methan::general_endianness(u); output.write(reinterpret_cast<const char*>(&_u), sizeof(int64_t)); } };
    template<> struct __serde<float> { inline void deserialize(std::istream& input, float& u) { float _u; input.read(reinterpret_cast<char*>(&_u), sizeof(float)); u = Methan::general_endianness(_u); } inline void serialize(std::ostream& output, const float& u) { const float _u = Methan::general_endianness(u); output.write(reinterpret_cast<const char*>(&_u), sizeof(float)); } };
    template<> struct __serde<double> { inline void deserialize(std::istream& input, double& u) { double _u; input.read(reinterpret_cast<char*>(&_u), sizeof(double)); u = Methan::general_endianness(_u); } inline void serialize(std::ostream& output, const double& u) { const double _u = Methan::general_endianness(u); output.write(reinterpret_cast<const char*>(&_u), sizeof(double)); } };

    template<>
    struct __serde<std::string>
    {
        static_assert(sizeof(std::string::value_type) == 1, "Methan require std::string to be made up of char (does not support UTF-16 yet).");

        inline void deserialize(std::istream& input, std::string& u)
        {
            uint32_t _size;
            __serde<uint32_t>().deserialize(input, _size);
            u.resize((size_t) _size);
            input.read(u.data(), u.size());
        }

        inline void serialize(std::ostream& output, const std::string& u)
        {
            uint32_t _size = (uint32_t) u.size();
            __serde<uint32_t>().serialize(output, _size);
            output.write(u.data(), _size);
        }
    };

    template<typename T, size_t _len>
    struct __serde<std::array<T, _len>>
    {
        inline void deserialize(std::istream& input, std::array<T, _len>& _array)
        {
            for (size_t i = 0; i < _len; ++i) __serde<T>().deserialize(input, _array[i]);
        }

        inline void serialize(std::ostream& output, const std::array<T, _len>& _array)
        {
            for (size_t i = 0; i < _len; ++i) __serde<T>().serialize(output, _array[i]);
        }
    };

    template<typename T, typename _Alloc>
    struct __serde<std::vector<T, _Alloc>>
    {
        inline void deserialize(std::istream& input, std::vector<T, _Alloc>& _vector)
        {
            uint32_t _size;
            __serde<uint32_t>().deserialize(input, _size);
            _vector.resize((size_t) _size);
            for (size_t i = 0; i < _size; ++i) __serde<T>().deserialize(input, _vector[i]);
        }

        inline void serialize(std::ostream& output, const std::vector<T, _Alloc>& _vector)
        {
            uint32_t _size = (uint32_t) _vector.size();
            __serde<uint32_t>().serialize(output, _size);
            for (size_t i = 0; i < _size; ++i) __serde<T>().serialize(output, _vector[i]);
        }
    };

    template<typename E>
    struct __serde<E, std::enable_if_t<std::is_enum<E>::value>>
    {
        inline void deserialize(std::istream& input, E& _enum)
        {
            std::underlying_type<E>::type e;
            Methan::Serde::deserialize<std::underlying_type<E>::type>(input, e);
            _enum = static_cast<E>(e);
        }

        inline void serialize(std::ostream& output, const E& _enum)
        {
            std::underlying_type<E>::type e = static_cast<std::underlying_type<E>::type>(_enum);
            Methan::Serde::serialize<std::underlying_type<E>::type>(output, e);
        }
    };

}
