#pragma once

#include <type_traits>
#include <functional>

#include <methan/core/except.hpp>
#include <methan/utility/assertion.hpp>


namespace Methan {

    class Varient
    {
        static constexpr uint8_t IsConstant = 0x01;
        static constexpr uint8_t IsDataOwner = 0x02;

    public:
        inline Varient(std::nullptr_t) noexcept
        : m_data(nullptr),
        METHAN_DEBUG_ONLY(m_dataName(nullptr),)
        m_typeId(0),
        m_flag(0),
        m_destructor(nullptr),
        m_copy(nullptr)
        {}

        inline Varient(const Varient& other)
        : m_data(other.m_data),
        METHAN_DEBUG_ONLY(m_dataName(other.m_dataName),)
        m_typeId(other.m_typeId),
        m_flag(other.m_flag),
        m_destructor(other.m_destructor),
        m_copy(other.m_copy)
        {
            if(m_flag & IsDataOwner) m_copy(&m_data, other.m_data);
        }

        template<typename T, std::enable_if_t<std::is_pointer<T>::value && std::is_const<std::remove_pointer_t<T>>::value, bool> = true>
        inline Varient(T constPtr) noexcept
        : m_data(reinterpret_cast<void*>(const_cast<std::remove_const_t<std::remove_pointer_t<T>>*>(constPtr))),
        METHAN_DEBUG_ONLY(m_dataName(typeid(T).name()),)
        m_typeId(typeid(T).hash_code()),
        m_flag(IsConstant),
        m_destructor(nullptr),
        m_copy(nullptr)
        {}

        template<typename T, std::enable_if_t<std::is_pointer<T>::value && !std::is_const<std::remove_pointer_t<T>>::value, bool> = true>
        inline Varient(T ptr) noexcept
        : m_data(reinterpret_cast<void*>(ptr)),
        METHAN_DEBUG_ONLY(m_dataName(typeid(T).name()),)
        m_typeId(typeid(T).hash_code()),
        m_flag(0),
        m_destructor(nullptr),
        m_copy(nullptr)
        {}

        template<typename T, std::enable_if_t<(!std::is_pointer<T>::value) && std::is_copy_constructible<T>::value, bool> = true>
        inline Varient(const T& ref)
        : m_data(new T(ref)),
        METHAN_DEBUG_ONLY(m_dataName(typeid(T).name()),)
        m_typeId(typeid(T).hash_code()),
        m_flag(IsDataOwner),
        m_destructor([](void* _data) { delete reinterpret_cast<T*>(_data); }),
        m_copy([](void** dest, void* _data) { *dest = new T(*reinterpret_cast<T*>(_data)); })
        {}

        inline ~Varient()
        {
            __destruct();
        }


        inline Varient& operator=(const Varient& other)
        {
            __destruct();

            METHAN_DEBUG_ONLY(m_dataName = other.m_dataName;)
            m_typeId = other.m_typeId;
            m_flag = other.m_flag;
            m_destructor = other.m_destructor;
            m_copy = other.m_copy;
            m_data = other.m_data;

            if(m_flag & IsDataOwner) m_copy(&m_data, other.m_data);

            return *this;
        }

        
        inline bool is_empty() const noexcept
        {
            return m_typeId == 0x0;
        }

        inline size_t type_id() const noexcept
        {
            return m_typeId;
        }

        template<typename T>
        inline bool is() const noexcept
        {
            if (is_empty()) return false;
            if (m_typeId == typeid(T).hash_code()) return true;

            if(std::is_pointer<T>::value && !(m_flag & IsConstant))
            {
                return m_typeId == typeid(std::remove_const_t<std::remove_pointer_t<T>>*).hash_code();
            }

            return false;
        }

        template<typename T, std::enable_if_t<std::is_pointer<T>::value && std::is_const<std::remove_pointer_t<T>>::value, bool> = true>
        inline T get() const
        {
            METHAN_ASSERT(!is_empty(), Methan::ExceptionType::IllegalArgument, "The call to `get` failed as the Varient is currently empty");
            METHAN_ASSERT(is<T>(), Methan::ExceptionType::BadCastException, "Cannot cast from type " METHAN_DEBUG_OR_RELEASE("`" + std::string(m_dataName) + "`", + std::to_string(m_typeId) + ) " to type " METHAN_DEBUG_OR_RELEASE("`" + std::string(typeid(T).name()) + "`", + std::to_string(typeid(T).hash_code())));
            return reinterpret_cast<T>(m_data);
        }

        template<typename T, std::enable_if_t<std::is_pointer<T>::value && !std::is_const<std::remove_pointer_t<T>>::value, bool> = true>
        inline T get() const
        {
            METHAN_ASSERT(!is_empty(), Methan::ExceptionType::IllegalArgument, "The call to `get` failed as the Varient is currently empty");
            METHAN_ASSERT(is<T>(), Methan::ExceptionType::BadCastException, "Cannot cast from type " METHAN_DEBUG_OR_RELEASE("`" + std::string(m_dataName) + "`", + std::to_string(m_typeId) + ) " to type " METHAN_DEBUG_OR_RELEASE("`" + std::string(typeid(T).name()) + "`", + std::to_string(typeid(T).hash_code())));
            METHAN_ASSERT(!(m_flag & IsConstant), Methan::ExceptionType::BadCastException, "Cannot cast a pointer-to-constant to a pointer-to-non-const");
            return reinterpret_cast<T>(m_data);
        }

        template<typename T, std::enable_if_t<(!std::is_pointer<T>::value) && std::is_copy_constructible<T>::value, bool> = true>
        inline const T& get() const
        {
            METHAN_ASSERT(!is_empty(), Methan::ExceptionType::IllegalArgument, "The call to `get` failed as the Varient is currently empty");
            METHAN_ASSERT(is<T>(), Methan::ExceptionType::BadCastException, "Cannot cast from type " METHAN_DEBUG_OR_RELEASE("`" + std::string(m_dataName) + "`", + std::to_string(m_typeId) + ) " to type " METHAN_DEBUG_OR_RELEASE("`" + std::string(typeid(T).name()) + "`", + std::to_string(typeid(T).hash_code())));
            return *reinterpret_cast<T*>(m_data);
        }

        template<typename T, std::enable_if_t<(!std::is_pointer<T>::value) && std::is_copy_constructible<T>::value, bool> = true>
        inline T& get()
        {
            METHAN_ASSERT(!is_empty(), Methan::ExceptionType::IllegalArgument, "The call to `get` failed as the Varient is currently empty");
            METHAN_ASSERT(is<T>(), Methan::ExceptionType::BadCastException, "Cannot cast from type " METHAN_DEBUG_OR_RELEASE("`" + std::string(m_dataName) + "`", + std::to_string(m_typeId) + ) " to type " METHAN_DEBUG_OR_RELEASE("`" + std::string(typeid(T).name()) + "`", + std::to_string(typeid(T).hash_code())));
            return *reinterpret_cast<T*>(m_data);
        }

    private:
        inline void __destruct()
        {
            if(m_flag & IsDataOwner) m_destructor(m_data);
        }

        size_t m_typeId;
        void* m_data;
        uint8_t m_flag;
        std::function<void(void*)> m_destructor;
        std::function<void(void**, void*)> m_copy;
        METHAN_DEBUG_ONLY(const char* m_dataName;)
    };

}
