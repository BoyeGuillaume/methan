#pragma once

#include <type_traits>
#include <vector>

#include <methan/core/except.hpp>
#include <methan/utility/assertion.hpp>

namespace Methan {

    template<typename T, typename _Alloc = std::allocator<T>>
    class Matrix
    {
    public:
        inline Matrix(size_t nrows, size_t ncols, const T& default_value)
        : m_nrows(nrows),
          m_ncols(ncols),
          m_container(nrows * ncols, default_value)
        {
            METHAN_ASSERT_ARGUMENT(nrows > 0 && ncols > 0);
        }

        template<typename std::enable_if<std::is_constructible<T>::value>::type = true>
        inline Matrix(size_t nrows, size_t ncols)
        : Matrix(nrows, ncols, T{})
        { }

        inline size_t ncols() const noexcept
        {
            return m_ncols;
        }

        inline size_t nrows() const noexcept
        {
            return m_nrows;
        }

        inline const std::vector<T, _Alloc>& internal_container() const noexcept
        {
            return m_container;
        }

        inline const T& get(size_t i, size_t j) const
        {
            return m_container[get_offset(i, j)];
        }

        inline T& get(size_t i, size_t j)
        {
            return m_container[get_offset(i, j)];
        }

        inline const T& operator()(size_t i, size_t j)
        {
            return m_container[get_offset(i, j)];
        }

        inline T& operator()(size_t i, size_t j)
        {
            return m_container[get_offset(i, j)];
        }

        inline size_t size() const noexcept
        {
            return m_ncols * m_nrows;
        }

    private:
        inline size_t get_offset(size_t i, size_t j) const 
        {
            METHAN_ASSERT_ARGUMENT(i < m_nrows && j < m_ncols);
            return i + m_nrows * j;
        }

        size_t m_nrows;
        size_t m_ncols;
        std::vector<T, _Alloc> m_container;
    };

}
