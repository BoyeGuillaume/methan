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
#include <methan/private/framework/threading/task.hpp>


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
        NotTerminated = AbstractTask::NotTerminated,

        /**
         * @brief The dataflow is terminated (due to either an error or the success of the operation)
         */
        Done = 1 << 3,

        /**
         * @brief The dataflow has been aborted due to an error
         */
        ErrorState = AbstractTask::FlashingError | Done,

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


    class AbstractDataFlow : public AbstractTask
    {
        friend class AbstractDataFlowFactory;
        METHAN_DISABLE_COPY_MOVE(AbstractDataFlow);

    protected:
        METHAN_API AbstractDataFlow(Context context,
                                    DataBlock* source,
                                    DataBlock* destination,
                                    std::vector<FlowPosition> sourceSites,
                                    std::vector<FlowPosition> destinationSites,
                                    AbstractDataFlowFactory* factory,
                                    Uuid uuid,
                                    const std::string& name);

        virtual EDataFlowStateFlags __run() = 0;

        /**
         * @brief internally perform the abort operation
         * @warning This method ain't thread safe and may be called within an unsafe context.
         *          It therefore can be called after that the thread has been terminated.
         */
        METHAN_API virtual void __abort();

        METHAN_API uint32_t run() override;

    public:
        METHAN_API virtual ~AbstractDataFlow();
        
        /**
         * @brief This methods will abort an copy operation. Notice it may only be called if the corresponding
         * flag is supported and if the flow has been initiated. 
         * 
         * Notice that if the operation has already been terminated this will not throw.
         */
        METHAN_API void abort();

        inline EDataFlowStateFlags state()
        {
            return EDataFlowStateFlags(static_cast<EDataFlowStateFlag>(signal()->state()));
        }

        inline bool running()
        {
            auto flag = state();
            return (flag & EDataFlowStateFlag::NotTerminated) &&
                   (flag & EDataFlowStateFlag::Initiate);
        }

        inline bool terminated()
        {
            return state() & EDataFlowStateFlag::Done;
        }

        inline bool successfull()
        {
            return (state() & EDataFlowStateFlag::Successfull) == EDataFlowStateFlag::Successfull;
        }

        inline bool flashingError()
        {
            return (state() & EDataFlowStateFlag::ErrorState) == EDataFlowStateFlag::ErrorState;
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
        const std::vector<FlowPosition> m_sourceSites;
        const std::vector<FlowPosition> m_destinationSites;
    
    private:
        DataBlock* m_source;
        DataBlock* m_destination;
        AbstractDataFlowFactory* m_factory;
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
                                                std::vector<FlowPosition> destinationSites,
                                                const Uuid& uuid) = 0;

    public:
        METHAN_API AbstractDataFlow* initiate_flow(DataBlock* source,
                                                   DataBlock* destination,
                                                   std::vector<FlowPosition> sourceSites,
                                                   std::vector<FlowPosition> destinationSites,
                                                   const Uuid& uuid);

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
