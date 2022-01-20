#pragma once

#include <type_traits>

#include <methan/core/except.hpp>


namespace Methan {

    template<typename T>
    inline T lerp(T t, T t0, T t1)
    {
        return (T(1) - t) * t0 + t * t1;
    }

    template<typename T>
    inline typename std::enable_if<std::is_integral<T>::value, T>::type xorshift(const T& n, int i)
    {
        return n ^ (n >> i);
    }

    template<typename T>
    inline typename std::enable_if<std::is_integral<T>::value, T>::type hash(const T&);

    template<>
    inline typename std::enable_if<std::is_integral<uint32_t>::value, uint32_t>::type hash<uint32_t>(const uint32_t& x)
    {
        constexpr uint32_t p = 0x55555555UL; // pattern of alterning 0 and 1
        constexpr uint32_t c = 3423571495UL; // random uneven integer constant
        return c * xorshift<uint32_t>(p * xorshift<uint32_t>(x, 16), 16);
    }
    
    template<>
    inline typename std::enable_if<std::is_integral<uint64_t>::value, uint64_t>::type hash<uint64_t>(const uint64_t& x)
    {
        return xorshift<uint64_t>(xorshift<uint64_t>(xorshift<uint64_t>(x + 0x9e3779b97f4a7c15, 30) * 0xbf58476d1ce4e5b9, 27) * 0x94d049bb133111eb, 31);
    }

    template<typename T, typename S>
    inline typename std::enable_if<std::is_unsigned<T>::value,T>::type
    constexpr rotl(const T n, const S i)
    {
        const T m = (std::numeric_limits<T>::digits - 1);
        const T c = i & m;
        return (n << c) | (n >> ((T(0) - c) & m));
    }

    inline size_t combine_hash(const size_t a, const size_t b)
    {
        return rotl(a, 12) ^ b;
    }

}

namespace std
{
    template<>
    struct hash<std::vector<uint32_t>>
    {
        size_t operator()(const std::vector<uint32_t>& xs) const
        {
            std::size_t ret = 0;
            for(auto& i : xs) ret = (size_t) Methan::rotl(ret, 11) ^ (size_t) Methan::hash(i);
            return ret;
        }
    };

    template<>
    struct hash<std::vector<uint64_t>>
    {
        size_t operator()(const std::vector<uint64_t>& xs) const
        {
            std::size_t ret = 0;
            for(auto& i : xs) ret = (size_t) Methan::rotl(ret, 11) ^ (size_t) Methan::hash(i);
            return ret;
        }
    };

#ifdef METHAN_OS_MACOS
    template<>
    struct hash<std::vector<size_t>>
    {
        size_t operator()(const std::vector<size_t>& xs) const
        {
            std::size_t ret = 0;
            for(auto& i : xs) ret = (size_t) Methan::rotl(ret, 11) ^ (size_t) Methan::hash<uint64_t>((uint64_t) i);
            return ret;
        }
    };
#endif

    template<typename A, typename B>
    struct hash<std::pair<A, B>>
    {
        size_t operator()(const std::pair<A, B>& p) const
        {
            std::size_t ret = Methan::combine_hash(std::hash<A>()(p.first), std::hash<B>()(p.second));
            return ret;
        }
    };
}
