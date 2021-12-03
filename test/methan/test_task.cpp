#include <catch2/catch_test_macros.hpp>
#include <methan/private/framework/threading/scheduler.hpp>
#include <methan/private/framework/threading/task.hpp>
#include <methan/private/private_context.hpp>
#include <methan/core/context.hpp>
#include <thread>
#include <chrono>


using namespace std::literals;
using namespace Methan;
typedef unsigned long long ull;

class FiboTask : public AbstractTask
{
public:
    FiboTask(Context context, ull* output, size_t n)
    : AbstractTask(context, generate_uuid(context), "Fibo"),
    m_output(output),
    m_n(n)
    {}

protected:
    virtual uint32_t run() override
    {
        ull result;
        ull previous = 0;
        *m_output = 1;
        for(size_t i = 1; i < m_n; ++i)
        {
            std::this_thread::sleep_for(50ms);
            result = previous + *m_output;
            previous = *m_output;
            *m_output = result;
        }

        return 0;
    }
    
private:
    ull* m_output;
    size_t m_n;
};

TEST_CASE("Task work correctly", "[threading]") {
    Context context = ContextBuilder()
        .add_logger_stdout(ELogLevel::Debug)
        .build();

    ull output = 0;
    FiboTask *task = new FiboTask(context, &output, 20);
    auto signal = task->signal();

    std::thread thread(&FiboTask::start, task);
    Scheduler::set_thread_priority(&thread, ThreadPriority::TimeCritical);
    signal->wait([](uint32_t value){ return value != AbstractTask::NotTerminated; });  

    REQUIRE(signal->state() != AbstractTask::NotTerminated);
    REQUIRE(output == 6765);

    thread.join();
    delete task;
    signal->signal(0x0);
    signal = nullptr;

    Methan::free(context);
}