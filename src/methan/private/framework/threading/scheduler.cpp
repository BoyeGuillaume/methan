#include <methan/private/framework/threading/scheduler.hpp>
#include <methan/private/private_context.hpp>
#include <methan/private/private_formatter.hpp>
#include <methan/utility/assertion.hpp>
#include <methan/utility/math.hpp>

#ifdef METHAN_OS_WINDOWS
 #include <windows.h>
#endif

#ifdef METHAN_OS_UNIX_LIKE
 #include <pthread.h>
#endif

METHAN_API Methan::Scheduler::Scheduler(Context context)
: Contextuable(context)
{
    METHAN_LOG_DEBUG(context->logger, "Scheduler has been created");
}

METHAN_API Methan::Scheduler::~Scheduler()
{
    METHAN_LOG_DEBUG(context()->logger, "Scheduler has been destructed");
}

METHAN_API void Methan::Scheduler::add_task(AbstractTask* task)
{
    std::lock_guard guard(m_tasks_mutex);
    METHAN_ASSERT_NON_NULL(task);
    METHAN_ASSERT_ARGUMENT(m_tasks.count(task) == 0);

    m_tasks.insert(task);
}

METHAN_API void Methan::Scheduler::remove_task(AbstractTask* task)
{
    std::lock_guard guard(m_tasks_mutex);
    METHAN_ASSERT_NON_NULL(task);
    METHAN_ASSERT_ARGUMENT(m_tasks.count(task) > 0);

    m_tasks.erase(task);
}

METHAN_API void Methan::Scheduler::set_thread_priority(std::thread* thread, ThreadPriority priority)
{
#ifdef METHAN_OS_WINDOWS
    HANDLE thread_handle = thread->native_handle();
    int native_priority = 0;

    switch(priority) {
    case ThreadPriority::AboveNormal:
        native_priority = THREAD_PRIORITY_ABOVE_NORMAL;
        break;
    
    case ThreadPriority::BelowNormal:
        native_priority = THREAD_PRIORITY_BELOW_NORMAL;
        break;

    case ThreadPriority::Highest:
        native_priority = THREAD_PRIORITY_HIGHEST;
        break;

    case ThreadPriority::Idle:
        native_priority = THREAD_PRIORITY_IDLE;
        break;

    case ThreadPriority::Lowest:
        native_priority = THREAD_PRIORITY_LOWEST;
        break;

    case ThreadPriority::Normal:
        native_priority = THREAD_PRIORITY_NORMAL;
        break;

    case ThreadPriority::TimeCritical:
        native_priority = THREAD_PRIORITY_TIME_CRITICAL;
        break;
    }

    SetThreadPriority(thread_handle, native_priority);
#endif


#ifdef METHAN_OS_UNIX_LIKE
    struct sched_param param;
    int policy;
    int native_priority = 0;
    pthread_t thId = thread->native_handle();

    switch(priority) {
    case ThreadPriority::AboveNormal:
        native_priority = 70;
        break;
    
    case ThreadPriority::BelowNormal:
        native_priority = 35;
        break;

    case ThreadPriority::Highest:
        native_priority = 90;
        break;

    case ThreadPriority::Idle:
        native_priority = 0;
        break;

    case ThreadPriority::Lowest:
        native_priority = 10;
        break;

    case ThreadPriority::Normal:
        native_priority = 50;
        break;

    case ThreadPriority::TimeCritical:
        native_priority = 100;
        break;
    }

    pthread_getschedparam(thId, &policy, &param);
    param.sched_priority = lerp<int>(native_priority, sched_get_priority_min(policy), sched_get_priority_max(policy)) / 100;
    pthread_setschedparam(thId, policy, &param);
#endif
}
