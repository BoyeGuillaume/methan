#pragma once

#include <vector>
#include <string>

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
         * @brief Compute the offset in the matrix of a given indices
         * 
         * @param indices The indices of the element we are conciderring
         * @return uint64_t The offset of this element in the vectorized form.
         */
        inline uint64_t offset_of(const std::vector<uint32_t>& indices) const
        {
            METHAN_ASSERT_ARGUMENT(indices.size() == rank());

            uint64_t offset = 0x0;
            uint64_t stride = 0x1;
            for(uint32_t i = 0; i < rank(); ++i)
            {
                METHAN_ASSERT_INDEX(indices[i], m_shape[i]);
                offset += stride * (uint64_t) indices[i];
                stride *= m_shape[i];
            }

            return offset;
        }

        /**
         * @brief Inverse method of `offset_of`. Compute the indices corresponding with a given offset
         * 
         * @param offset the offset of the element in the vectorized form
         * @return std::vector<uint32_t> the corresponding matrix
         */
        inline std::vector<uint32_t> indices_of(uint64_t offset) const
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

        /**
         * @brief Convert the given tensor_shape to a strin
         * 
         * @return std::string the string representation of the current tensor_shape
         */
        inline std::string to_string() const
        {
            std::string output = "{";
            for(size_t i = 0; i < rank(); ++i) {
                output += std::to_string(m_shape[i]);
                if(i != rank() - 1) output += ", ";
            }
            return output + "}";
        }

    protected:
        std::vector<uint32_t> m_shape;
        uint32_t m_rank;
        uint64_t m_size;

        METHAN_SERDE_GENERATOR(TensorShape, m_shape, m_rank, m_size);
    };

    class SlicedTensorShape : public TensorShape
    {
    public:
        inline SlicedTensorShape(const TensorShape& parent, std::vector<uint32_t> offsets, std::vector<uint32_t> shape)
        : TensorShape(shape),
        m_offsets(std::move(offsets)),
        m_parentShape(parent.shape()),
        m_parentSize(parent.size())
        {
            METHAN_ASSERT_ARGUMENT(m_offsets.size() == m_parentShape.size() && m_shape.size() == m_offsets.size());

#ifdef METHAN_EXPAND_ASSERTION
            for(size_t i = 0; i < m_rank; ++i)
            {
                METHAN_ASSERT_ARGUMENT(m_shape[i] > 0 && m_shape[i] + m_offsets[i] <= m_parentShape[i]);
            }
#endif
        }

        inline SlicedTensorShape(const TensorShape& parent, std::initializer_list<uint32_t> offsets, std::initializer_list<uint32_t> shape)
        : SlicedTensorShape(parent, std::vector<uint32_t>(offsets), std::vector<uint32_t>(shape))
        { }

        /**
         * @brief Return the list of offsets for each dimension from the original tensors
         * 
         * @return const std::vector<uint32_t> the list of offsets relative to the original tensor
         */
        inline const std::vector<uint32_t>& offsets() const noexcept
        {
            return m_offsets;
        }

        /**
         * @brief Return the shape of the parent
         */
        inline const std::vector<uint32_t>& parent_shape() const noexcept
        {
            return m_parentShape;
        }

        /**
         * @brief Return the size of the parent
         * 
         * @return const uint64_t size of the original tensor
         */
        inline const uint64_t parent_size() const noexcept
        {
            return m_parentSize;
        }

        /**
         * @brief Get the offset corresponding to the given indices in the parent tensor. THIS IS NOT THE SAME AS offset_of !!
         * 
         * @param indices a list of all the indices (must of size equal to the rank)
         * @return uint64_t the corresponding offset in the parent tensor
         */
        inline uint64_t absolute_offset_of(const std::vector<uint32_t>& indices) const
        {
            METHAN_ASSERT_ARGUMENT(indices.size() == rank());

            uint64_t offset = 0x0;
            uint64_t stride = 0x1;
            for(uint32_t i = 0; i < rank(); ++i)
            {
                METHAN_ASSERT_INDEX(indices[i], m_shape[i]);
                offset += stride * (uint64_t) (m_offsets[i] + indices[i]);
                stride *= m_parentShape[i];
            }

            return offset;
        }

        /**
         * @brief Inverse method of `absolute_offset_of`. Compute the indices corresponding to a given offset in the parent tensor
         * 
         * @param offset the offset of the element in the parent vectorized-tensor
         * @return std::vector<uint32_t> the corresponding indices in this tensor
         */
        inline std::vector<uint32_t> absolute_indices_of(uint64_t offset)
        {
            METHAN_ASSERT_INDEX(offset, m_parentSize);
            std::vector<uint32_t> indices(rank(), 0);

            uint64_t stride = parent_size();
            for(uint32_t i = rank() - 1; i != std::numeric_limits<uint32_t>::max(); --i) // waiting for an overflow to occcure
            {
                stride /= m_parentShape[i];
                indices[i] = (uint32_t) (offset / stride - m_offsets[i]);
                METHAN_ASSERT_INDEX(indices[i], m_shape[i]);
                offset -= stride * (uint64_t) (m_offsets[i] + indices[i]);
            }

            return indices;
        }

    private:
        std::vector<uint32_t> m_offsets;
        std::vector<uint32_t> m_parentShape;
        uint64_t m_parentSize;

        METHAN_SERDE_GENERATOR(SlicedTensorShape, m_shape, m_rank, m_size, m_offsets, m_parentShape, m_parentSize);
    };

}
