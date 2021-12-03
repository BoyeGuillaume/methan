#pragma once

#include <mutex>
#include <unordered_set>

#include <methan/core/except.hpp>
#include <methan/core/context.hpp>
#include <methan/private/framework/threading/task.hpp>
#include <methan/utility/uuid.hpp>


namespace Methan {

    enum class ThreadPriority : uint8_t
    {
        AboveNormal,
        BelowNormal,
        Highest,
        Idle,
        Lowest,
        Normal,
        TimeCritical,
    };

    class Scheduler : public Contextuable
    {
    public:
        METHAN_API Scheduler(Context context);
        METHAN_API ~Scheduler();

        METHAN_API void add_task(AbstractTask* task);
        METHAN_API void remove_task(AbstractTask* task);

        METHAN_API static void set_thread_priority(std::thread* thread, ThreadPriority priority);
    private:

        std::mutex m_tasks_mutex;
        std::unordered_set<AbstractTask*> m_tasks;
    };

}


