#pragma once

#include <climits>

#include <methan/core/except.hpp>


namespace Methan {

    enum class Endianness {
        BigEndian,
        LittleEndian
    };

#if defined(METHAN_PREFER_LITTLE_ENDIAN)
    static constexpr Endianness preferred_endianness = Endianness::LittleEndian;
#else
    static constexpr Endianness preferred_endianness = Endianness::BigEndian;
#endif

    namespace __private__::Endianness {
        static constexpr uint32_t uint32_ = 0x01020304;
        static constexpr uint8_t magic_ = (const uint8_t&) uint32_;
        static constexpr bool little = magic_ == 0x04;
        static constexpr bool big = magic_ == 0x01;
        static_assert(little ^ big, "Cannot recognise the endianness of this system. Endianess not supported");
    };

    static constexpr Endianness system_endianness = __private__::Endianness::little ? Endianness::LittleEndian : Endianness::BigEndian;

    /**
     * @brief Swap the endianness of any type (by inverting its byte order).
     * @warning MUST ONLY BE CALLED BASIC OBJECT
     * 
     * @tparam T the type of the object considered.
     * @param u the input object
     * @return T same object with revert endianness
     */
    template<typename T>
    inline T swap_endian(T u)
    {
        static_assert(CHAR_BIT == 8, "Methan only support platform with bytes of size 8 bits");

        union
        {
            T u;
            unsigned char u8[sizeof(T)];
        } source, dest;
        source.u = u;

        for(size_t i = 0; i < sizeof(T); ++i)
            dest.u8[i] = source.u8[sizeof(T) - i - 1];
        
        return dest.u;
    }

#if defined(METHAN_USE_INTRINSIC)

#if defined(METHAN_COMPILER_MSC)
    template<>
    inline uint64_t swap_endian(uint64_t u)
    {
        return _byteswap_uint64(u);
    }

    template<>
    inline uint32_t swap_endian(uint32_t u)
    {
        return _byteswap_ulong(u);
    }

    template<>
    inline uint16_t swap_endian(uint16_t u)
    {
        return _byteswap_ushort(u);
    }
#elif defined(METHAN_COMPILER_GCC)
    template<>
    LUNAR_INLINE uint64_t swap_endian(uint64_t u) {
        return __builtin_bswap64(u);
    }

    template<>
    LUNAR_INLINE uint32_t swap_endian(uint32_t u) {
        return __builtin_bswap32(u);
    }
#endif
#endif

    template<>
    inline int16_t swap_endian(int16_t u) { return (int16_t) swap_endian<uint16_t>((uint16_t) u); }
    
    template<>
    inline int32_t swap_endian(int32_t u) { return (int32_t) swap_endian<uint32_t>((uint32_t) u); }
    
    template<>
    inline int64_t swap_endian(int64_t u) { return (int64_t) swap_endian<uint64_t>((uint64_t) u); }
    
    template<>
    inline uint8_t swap_endian(uint8_t u) { return u; }
    
    template<>
    inline int8_t swap_endian(int8_t u) { return u; }

    template<>
    inline double swap_endian(double u)
    {
        uint64_t mem = swap_endian<uint64_t>(* (uint64_t*) &u);
        return * (double*) &mem;
    }

    template<>
    inline float swap_endian(float u)
    {
        uint32_t mem = swap_endian<uint32_t>(* (uint32_t*) &u);
        return * (float*) &mem;
    }

    /**
     * @brief Generalize any type T (must be simple type) in order to provide 
     * a binary code for numbers that work accross platforms
     */
    template<typename T>
    inline T general_endianness(T u)
    {
        if(system_endianness == preferred_endianness) return u;
        else return swap_endian<T>(u);
    }

}
