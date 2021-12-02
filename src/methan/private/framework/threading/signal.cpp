#include <methan/private/framework/threading/signal.hpp>
#include <methan/private/private_context.hpp>
#include <methan/private/private_formatter.hpp>
#include <algorithm>



METHAN_API Methan::Signal::Signal(Context context, Uuid uuid)
: Contextuable(context),
  m_uuid(uuid),
  m_value(0x0)
{
    METHAN_LOG_DEBUG(context->logger, "Signal create with uuid {}", m_uuid);
}

METHAN_API Methan::Signal::~Signal()
{
    m_mutex.lock();
    for(AbstractSignalObserver* observers : m_observers)
    {
        observers->weakify();
    }
    m_mutex.unlock();

    METHAN_LOG_DEBUG(context()->logger, "Signal {} destructed", m_uuid);
}

METHAN_API void Methan::Signal::signal(uint32_t value)
{
    {
        std::lock_guard guard(m_mutex);

        // Even if the shared variable is atomic, it must be modified under 
        // the mutex in order to correctly publish the modification to the 
        // waiting thread. 
        m_value.store(value);
        
        for(AbstractSignalObserver* observer : m_observers) 
            observer->call(value);
    }
    m_notifier.notify_all();

    METHAN_LOG_DEBUG(context()->logger, "Signal {} signaled with value {}", m_uuid, value);
}

METHAN_API void Methan::Signal::wait(std::function<bool(uint32_t)> predicate)
{
    // Any thread that intends to wait on std::condition_variable has to 
    // 1. acquire a std::unique_lock<std::mutex>, on the same mutex as 
    //    used to protect the shared variable 
    // 2. execute wait, wait_for, or wait_until. The wait operations 
    //    atomically release the mutex and suspend the execution of the thread.
    // 3. When the condition variable is notified, a timeout expires, or a 
    //    spurious wakeup occurs, the thread is awakened, and the mutex is 
    //    atomically reacquired. The thread should then check the condition
    //    and resume waiting if the wake up was spurious. 
    // Note that this is done by the std library
    std::unique_lock<std::mutex> lock(m_mutex);
    m_notifier.wait(lock, [this, predicate]() { return predicate(this->m_value.load()); });
    lock.unlock();
}

METHAN_API std::cv_status Methan::Signal::wait_for(std::chrono::milliseconds ms, std::function<bool(uint32_t)> predicate)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    std::cv_status status = m_notifier.wait_for(lock, ms);
    lock.unlock();
    return status;
}

METHAN_API void Methan::Signal::push_observer(AbstractSignalObserver* observer)
{
    std::lock_guard guard(m_mutex);
    m_observers.push_back(observer);
}

METHAN_API void Methan::Signal::pop_observer(AbstractSignalObserver* observer)
{
    std::lock_guard guard(m_mutex);
    auto it = std::remove(m_observers.begin(), m_observers.end(), observer);
    METHAN_ASSERT_ARGUMENT(it != m_observers.end());
    m_observers.erase(it, m_observers.end());
}

METHAN_API Methan::AbstractSignalObserver::AbstractSignalObserver(Signal* signal)
: Contextuable(signal),
  m_signal(signal)
{
    m_signal->push_observer(this);
}

METHAN_API Methan::AbstractSignalObserver::~AbstractSignalObserver()
{
    m_signal->pop_observer(this);
}

METHAN_API void Methan::AbstractSignalObserver::weakify()
{
    m_signal = nullptr;
    onWeakified();
}

METHAN_API void Methan::AbstractSignalObserver::onWeakified()
{ }
