#pragma once

#include <mutex>

#include <methan/core/except.hpp>
#include <methan/core/contextuable.hpp>
#include <methan/core/serializable.hpp>
#include <methan/utility/uuid.hpp>
#include <methan/utility/enum.hpp>
#include <methan/utility/data_size.hpp>
#include <methan/private/framework/framework.hpp>
#include <methan/private/framework/memory/allocator.hpp>


namespace Methan {

    enum class EDataFlowPoliciesFlag : uint32_t
    {
        AsyncIOFlow = 1 << 0,
        SupportAbort = 1 << 1
    };

    enum class EDataFlowStateFlag : uint32_t
    {
        /**
         * @brief Whever or not the dataflow is async
         */
        AsyncIO = (uint32_t) EDataFlowPoliciesFlag::AsyncIOFlow,

        /**
         * @brief Whever or not the dataflow has started
         */
        Initiate = 1 << 1,
    
        /**
         * @brief Whever or not the dataflow has started and ain't terminated yet
         */
        InProgress = 1 << 2,

        /**
         * @brief The dataflow is terminated (due to either an error or the success of the operation)
         */
        Done = 1 << 3,

        /**
         * @brief The dataflow has been aborted due to an error
         */
        ErrorState = (1 << 4) | Done,

        /**
         * @brief The dataflow successfully terminated
         */
        Successfull = (1 << 5) | Done
    };

    typedef EnumFlag<EDataFlowPoliciesFlag> EDataFlowPoliciesFlags;
    typedef EnumFlag<EDataFlowStateFlag> EDataFlowStateFlags;
    METHAN_ENUMSET_OPERATORS(EDataFlowPoliciesFlags);
    METHAN_ENUMSET_OPERATORS(EDataFlowStateFlags);

    struct DataFlowFactoryDescriptor
    {
        EDataFlowPoliciesFlags flags;
        Uuid uuid;
        Uuid source;
        Uuid destination;

        METHAN_SERDE_GENERATOR(DataFlowFactoryDescriptor, flags, uuid, source, destination);
    };

    /**
     * @brief Describe one 'row' of any copy / move operations. It is describe by 4 parameters
     */
    struct FlowPosition
    {
        /**
         * @brief The number of times the copy is made
         */
        DataSize length;

        /**
         * @brief The length of one elements (in bytes) to copy
         */
        DataSize elementSize;

        /**
         * @brief The offset between the first element and the starting element that must be copied
         */
        DataSize offset;

        /**
         * @brief The offset between the start of two elements (in bytes). SHOULD NOT OVERLAP !!
         * 
         */
        DataSize stride;

        constexpr FlowPosition(DataSize length, DataSize offset, DataSize elementSize, DataSize stride)
        : length(length),
        elementSize(elementSize),
        offset(offset),
        stride(stride)
        { }

        constexpr FlowPosition(DataSize length, DataSize offset, DataSize elementSize)
        : FlowPosition(length, offset, elementSize, elementSize)
        { }

        constexpr FlowPosition(DataSize length, DataSize offset = 0_B)
        : FlowPosition(length, offset, 1_B)
        { }

        inline bool is_valid(DataSize blockSize) const noexcept
        {
            if(elementSize > stride) return false;
            return offset + (length - 1) * stride + elementSize <= blockSize;
        }

        inline bool is_trivially_copyable() const noexcept
        {
            return elementSize == stride;
        }

        METHAN_SERDE_GENERATOR(FlowPosition, length, offset, stride);
    };


    class AbstractDataFlow : public Contextuable
    {
        friend class AbstractDataFlowFactory;
        METHAN_DISABLE_COPY_MOVE(AbstractDataFlow);

        METHAN_API void mark_started();

    protected:
        METHAN_API AbstractDataFlow(Context context,
                                    DataBlock* source,
                                    DataBlock* destination,
                                    std::vector<FlowPosition> sourceSites,
                                    std::vector<FlowPosition> destinationSites,
                                    AbstractDataFlowFactory* factory);

        virtual void __start() = 0;
        virtual METHAN_API void __abort();

        METHAN_API void mark_interrupted();
        METHAN_API void mark_terminated();

    public:
        METHAN_API virtual ~AbstractDataFlow();
        METHAN_API void start();
        
        /**
         * @brief This methods will abort an copy operation. Notice it may only be called if the corresponding
         * flag is supported and if the flow has been initiated. 
         * 
         * Notice that if the operation has already been terminated this will not throw.
         */
        METHAN_API void abort();

        inline EDataFlowStateFlags flags() const noexcept
        {
            return m_flags;
        }

        inline bool terminated() const noexcept
        {
            return flags() & EDataFlowStateFlag::Done;
        }

        inline bool running() const noexcept
        {
            return flags() & EDataFlowStateFlag::InProgress;
        }

        inline bool successfull() const noexcept
        {
            return (flags() & EDataFlowStateFlag::Successfull) == EDataFlowStateFlag::Successfull;
        }

        inline bool interrupted() const noexcept
        {
            return (flags() & EDataFlowStateFlag::ErrorState) == EDataFlowStateFlag::ErrorState;
        }

        inline Uuid uuid() const
        {
            return m_uuid;
        }

        inline AbstractDataFlowFactory* factory() const noexcept
        {
            return m_factory;
        }

        inline DataBlock* source() const noexcept
        {
            return m_source;
        }

        inline DataBlock* destination() const noexcept
        {
            return m_destination;
        }

        inline const std::vector<FlowPosition>& source_sites() const noexcept
        {
            return m_sourceSites;
        }

        inline const std::vector<FlowPosition>& destination_sites() const noexcept
        {
            return m_destinationSites;
        }

    protected:
        EDataFlowStateFlags m_flags;
        std::recursive_mutex __flag_mutex;
        const std::vector<FlowPosition> m_sourceSites;
        const std::vector<FlowPosition> m_destinationSites;
    
    private:
        DataBlock* m_source;
        DataBlock* m_destination;
        AbstractDataFlowFactory* m_factory;
        Uuid m_uuid;
    };


    class AbstractDataFlowFactory : public Contextuable
    {
        METHAN_DISABLE_COPY_MOVE(AbstractDataFlowFactory);

    protected:
        DataFlowFactoryDescriptor m_descriptor;

        METHAN_API AbstractDataFlowFactory(Context context, Uuid source, Uuid destination);
        METHAN_API virtual ~AbstractDataFlowFactory();

        virtual AbstractDataFlow* __create_flow(DataBlock* source,
                                                DataBlock* destination,
                                                std::vector<FlowPosition> sourceSites,
                                                std::vector<FlowPosition> destinationSites) = 0;

    public:
        METHAN_API AbstractDataFlow* initiate_flow(DataBlock* source,
                                                   DataBlock* destination,
                                                   std::vector<FlowPosition> sourceSites,
                                                   std::vector<FlowPosition> destinationSites);

        inline const DataFlowFactoryDescriptor& descriptor() const noexcept
        {
            return m_descriptor;
        }

        inline const Uuid& uuid() const
        {
            return m_descriptor.uuid;
        }

        inline const Uuid& source() const
        {
            return m_descriptor.source;
        }

        inline const Uuid& destination() const
        {
            return m_descriptor.destination;
        }

    private:
        std::mutex __lock_flow_creation_m;
    };

}
