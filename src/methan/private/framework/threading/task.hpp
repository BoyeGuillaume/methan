#pragma once

#include <atomic>
#include <memory>

#include <methan/core/except.hpp>
#include <methan/core/contextuable.hpp>
#include <methan/utility/enum.hpp>
#include <methan/utility/uuid.hpp>
#include <methan/private/framework/threading/signal.hpp>


namespace Methan {

    class AbstractTask : public Contextuable
    {
    public:
        static constexpr uint32_t NotTerminated = 0xffffffff;

        METHAN_API AbstractTask(Context context, Uuid uuid, std::string name);
        METHAN_API virtual ~AbstractTask();

        inline const Uuid& uuid() const noexcept
        {
            return m_uuid;
        }

        inline std::shared_ptr<Signal> signal() 
        {
            return m_signal;
        }

        METHAN_API void start();
    
    protected:
        virtual uint32_t run() = 0;

    private:
        Uuid m_uuid;
        std::string m_name;
        std::atomic_int8_t m_flags;
        std::shared_ptr<Signal> m_signal;
    };

}

namespace std {

    template<>
    struct hash<Methan::AbstractTask*>
    {
        size_t operator()(const Methan::AbstractTask* task) const 
        {
            return task->uuid().hash();
        }
    };

}