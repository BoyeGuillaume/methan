#include <methan/private/framework/threading/task.hpp>
#include <methan/private/private_context.hpp>
#include <methan/private/private_formatter.hpp>
#include <mutex>


METHAN_API Methan::AbstractTask::AbstractTask(Context context, Uuid uuid, std::string name)
: Contextuable(context),
  m_uuid(uuid),
  m_name(name)
{
    m_signal = std::make_shared<Signal>(context, uuid, 0x0);
    context->task_mutex.lock();
    context->tasks.insert(std::make_pair(uuid, this));
    context->task_mutex.unlock();

    METHAN_LOG_DEBUG(context->logger, "New task {} with uuid {} registered", m_name, m_uuid);
}

METHAN_API Methan::AbstractTask::~AbstractTask()
{
    METHAN_LOG_DEBUG(context()->logger, "task with uuid {} deleted", m_uuid);
}

METHAN_API void Methan::AbstractTask::start()
{
    uint32_t result = 0;
    METHAN_LOG_INFO(context()->logger, "task {} with uuid {} started", m_name, m_uuid);

    try
    {
        result = run();
        result |= AbstractTask::Terminated | AbstractTask::Initiated;
    }
    catch (std::exception e)
    {
        METHAN_LOG_ERROR(context()->logger, "Task {} with uuid {} failed with error code {}", m_name, m_uuid, e.what());
        result = AbstractTask::Failure | AbstractTask::Initiated | AbstractTask::Terminated;
    }
    
    METHAN_LOG_INFO(context()->logger, "task {} with uuid {} terminated", m_name, m_uuid);
    m_signal->signal(result);
}

