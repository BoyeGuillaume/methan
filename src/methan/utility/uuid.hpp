#pragma once

#include <random>
#include <array>
#include <string>

#include <methan/core/except.hpp>
#include <methan/utility/endianness.hpp>
#include <methan/core/serializable.hpp>

namespace Methan {


   class Uuid {
   public:
      inline Uuid(const Uuid& other) noexcept
      {
         m_data[0] = other.m_data[0];
         m_data[1] = other.m_data[1];
         m_ps_data[0] = other.m_ps_data[0];
         m_ps_data[1] = other.m_ps_data[1];
      }

      inline Uuid(Uuid&& other) noexcept
      {
         m_data[0] = other.m_data[0];
         m_data[1] = other.m_data[1];
         m_ps_data[0] = other.m_ps_data[0];
         m_ps_data[1] = other.m_ps_data[1];
      }

      inline Uuid& operator=(const Uuid& other) noexcept
      {
         m_data[0] = other.m_data[0];
         m_data[1] = other.m_data[1];
         m_ps_data[0] = other.m_ps_data[0];
         m_ps_data[1] = other.m_ps_data[1];
         return *this;  
      }

      inline Uuid& operator=(Uuid&& other) noexcept
      {
         m_data[0] = other.m_data[0];
         m_data[1] = other.m_data[1];
         m_ps_data[0] = other.m_ps_data[0];
         m_ps_data[1] = other.m_ps_data[1];
         return *this;  
      }

      inline Uuid(std::nullptr_t)
      {
         m_data.fill(0x0);
         m_ps_data.fill(0x0);
      }

      inline Uuid() : Uuid(nullptr) {}

      inline Uuid(uint64_t upper, uint64_t lower)
      {
         m_data[0] = Methan::to_endianness(upper, Methan::Endianness::BigEndian);
         m_data[1] = Methan::to_endianness(lower, Methan::Endianness::BigEndian);
         m_ps_data[0] = upper;
         m_ps_data[1] = lower;
      }

      inline friend bool operator==(const Uuid& lhs, const Uuid& rhs) noexcept
      {
         return lhs.m_ps_data[0] == rhs.m_ps_data[0] &&
                lhs.m_ps_data[1] == rhs.m_ps_data[1];
      }

      inline friend bool operator!=(const Uuid& lhs, const Uuid& rhs) noexcept
      {
         return !operator==(lhs, rhs);
      }

      inline friend bool operator<(const Uuid& lhs, const Uuid& rhs) noexcept
      {
         return (lhs.m_ps_data[0] < rhs.m_ps_data[0]) ||
                (lhs.m_ps_data[0] == rhs.m_ps_data[0] && lhs.m_ps_data[1] < rhs.m_ps_data[1]);
      }

      inline friend bool operator>(const Uuid& lhs, const Uuid& rhs) noexcept
      {
         return !operator<(lhs, rhs) && !operator==(lhs, rhs);
      }

      inline friend bool operator>=(const Uuid& lhs, const Uuid& rhs) noexcept
      {
         return !operator<(lhs, rhs);
      }

      inline friend bool operator<=(const Uuid& lhs, const Uuid& rhs) noexcept
      {
         return operator<(lhs, rhs) || operator==(lhs, rhs);
      }

      inline std::string to_string() noexcept
      {
         static constexpr const char* hex_digit = "0123456789abcdef";
         std::string output(36, '\0');
         uint8_t index = 0;
         uint8_t* data = (uint8_t*) m_data.data();
         
         while(index <= 35)
         {
            // convert current byte into hex
            output[index++] = hex_digit[(*data >> 4) & 0xF];
            output[index++] = hex_digit[(*data >> 0) & 0xF];

            if(index == 8 || index == 13 || index == 18 || index == 23) {
               output[index++] = '-';
            }
            ++data;
         }

         return output;
      }

      inline size_t hash() const
      {
         return std::hash<uint64_t>()(m_ps_data[0]) ^ std::hash<uint64_t>()(m_ps_data[1]);
      }

      inline uint64_t _ps_upper() const noexcept
      {
         return m_ps_data[0];
      } 

      inline uint64_t _ps_lower() const noexcept
      {
         return m_ps_data[1];
      } 

   private:
      std::array<uint64_t, 2> m_data;
      std::array<uint64_t, 2> m_ps_data; // Platform specific = with platform endianness
      
      friend struct Methan::__private__::__serde<Uuid>;
      inline friend void __p_serde__deserialize(std::istream& input, Uuid& u)
      {
         std::array<uint64_t, 2> data;
         Methan::Serde::deserialize(input, data);
         u = Uuid(data[0], data[1]);
      }
      
      inline friend void __p_serde__serialize(std::ostream& output, const Uuid& u)
      {
         Methan::Serde::serialize(output, u.m_ps_data);
      }
   };

   class UuidFactory
   {
   public:
      inline UuidFactory()
      : m_uuid({0x05cf35ac1640730d, 0x52e7c918a6eccb2d})
      {
         __randomize();
      }

      inline Uuid generate()
      {
         Uuid _uuid = Uuid({m_uuid[0], m_uuid[1]});
         __randomize();
         return _uuid;
      }

      inline Uuid operator()()
      {
         return generate();
      }

   private:
      inline uint64_t __fast_hash_murmur64(uint64_t x)
      {
         x ^= x >> 33;
         x *= 0xff51afd7ed558ccd;
         x ^= x >> 33;
         x *= 0xc4ceb9fe1a85ec53;
         x ^= x >> 33;
         return x;
      }

      inline void __randomize()
      {
         for(size_t i = 0; i < 2; ++i)
         {
            m_uuid[0] = __fast_hash_murmur64(m_uuid[0]) ^ m_randgen();
            m_uuid[1] = __fast_hash_murmur64(m_uuid[1]) ^ m_randgen();
         }
      }

      std::array<uint64_t, 2> m_uuid;
      std::mt19937_64 m_randgen;
   };

}

namespace std {

   template<>
   struct hash<Methan::Uuid>
   {
      size_t operator()(const Methan::Uuid& uuid) const 
      {
         return uuid.hash();
      }
   };

}
