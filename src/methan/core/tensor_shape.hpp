#pragma once

#include <vector>

#include <methan/core/except.hpp>
#include <methan/core/serializable.hpp>
#include <methan/utility/assertion.hpp>


namespace Methan {

    class TensorShape
    {
    public:
        inline TensorShape(std::vector<uint32_t> shape)
        : m_shape(std::move(shape)),
        m_rank(0x0),
        m_size(0x1)
        {
            METHAN_ASSERT_ARGUMENT(m_shape.size() > 0);
            m_rank = (uint32_t) m_shape.size();

            for(size_t i = 0; i < m_rank; ++i)
            {
                METHAN_ASSERT_ARGUMENT(m_shape[i] > 0);
                METHAN_ASSERT_ARGUMENT(m_size * m_shape[i] >= m_size); // Prevent overflow
                m_size *= m_shape[i];
            }
        }

        inline TensorShape(std::initializer_list<uint32_t> shape)
        : TensorShape(std::vector<uint32_t>(shape))
        { }

        /**
         * @brief Compute the offset in the matrix of a given index
         * 
         * @param index The index of the element we are conciderring
         * @return uint64_t The offset of this element in the vectorized form.
         */
        inline uint64_t offset_of(const std::vector<uint32_t>& index) const
        {
            METHAN_ASSERT_ARGUMENT(index.size() == rank());

            uint64_t offset = 0x0;
            uint64_t stride = 0x1;
            for(uint32_t i = 0; i < rank(); ++i)
            {
                METHAN_ASSERT_INDEX(index[i], m_shape[i]);
                offset += stride * (uint64_t) index[i];
                stride *= m_shape[i];
            }

            return offset;
        }

        /**
         * @brief Inverse method of `offset_of`. Compute the index corresponding with a given offset
         * 
         * @param offset the offset of the element in the vectorized form
         * @return std::vector<uint32_t> the corresponding matrix
         */
        inline std::vector<uint32_t> index_of(uint64_t offset) const
        {
            METHAN_ASSERT_INDEX(offset, size());
            std::vector<uint32_t> indices(rank(), 0);

            uint64_t stride = size();
            for(uint32_t i = rank() - 1; i != std::numeric_limits<uint32_t>::max(); --i) // waiting for an overflow to occcure
            {
                stride /= m_shape[i];
                indices[i] = (uint32_t) (offset / stride);
                offset -= indices[i] * stride;
            }

            return indices;
        }

        /**
         * @brief Return a const-reference to the shape of this tensor
         * 
         * @return const std::vector<uint32_t>& The shape of this tensor (along each axis)
         */
        inline const std::vector<uint32_t>& shape() const noexcept
        {
            return m_shape;
        }

        /**
         * @brief The rank of the tensor (number of dimensions)
         * 
         * @return uint32_t the number of dimensions of this tensor (equivalent to shape().size())
         */
        inline uint32_t rank() const noexcept
        {
            return m_rank;
        }

        /**
         * @brief Number of elements contains in the tensor. (Not necessarly in bytes)
         * 
         * @return uint64_t the number of elements that are containned by the tensor
         */
        inline uint64_t size() const noexcept
        {
            return m_size;
        }

    private:
        std::vector<uint32_t> m_shape;
        uint32_t m_rank;
        uint64_t m_size;
    };

}
