#pragma once

#include <atomic>
#include <mutex>
#include <list>
#include <condition_variable>
#include <chrono>

#include <methan/core/except.hpp>
#include <methan/core/contextuable.hpp>
#include <methan/utility/enum.hpp>
#include <methan/utility/uuid.hpp>


namespace Methan {

    class AbstractSignalObserver;

    class Signal : public Contextuable
    {
        friend class AbstractSignalObserver;

    public:
        METHAN_API Signal(Context context, Uuid uuid);
        METHAN_API ~Signal();

        METHAN_API void signal(uint32_t value);

        /**
         * @brief Wait until a certain predicate is verify. Notice that this will suspend the thread until the signal is notify (except on timeout & spurious wakeup)
         * 
         * @param predicate the predicate that enable the thread to resume
         */
        METHAN_API void wait(std::function<bool(uint32_t)> predicate);
        METHAN_API std::cv_status wait_for(std::chrono::milliseconds ms, std::function<bool(uint32_t)> predicate);
        
        inline uint32_t state() const
        {
            return m_value.load();
        }

    private:
        METHAN_API void push_observer(AbstractSignalObserver* observer);
        METHAN_API void pop_observer(AbstractSignalObserver* observer);

        std::mutex m_mutex;
        std::atomic_uint32_t m_value;
        std::list<AbstractSignalObserver*> m_observers;
        std::condition_variable m_notifier;
        Uuid m_uuid;
    };

    class AbstractSignalObserver : public Contextuable
    {
        friend class Signal;

    public:
        METHAN_API AbstractSignalObserver(Signal* signal);
        METHAN_API virtual ~AbstractSignalObserver();

    protected:
        /**
         * @brief Method called by the signal when he is signaled
         * @warning This methods MUST NOT modify the signal (by creating new signals) as this
         *          would create a dead-lock.
         * 
         * @param value the new value of the signal
         */
        virtual void call(uint32_t value) = 0;

        METHAN_API virtual void onWeakified();

        inline Signal* signal()
        {
            return m_signal;
        }
    
    private:
        METHAN_API void weakify();

        Signal* m_signal;
    };

}
