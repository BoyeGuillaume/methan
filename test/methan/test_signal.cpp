#include <catch2/catch_test_macros.hpp>
#include <methan/core/context.hpp>
#include <methan/private/private_context.hpp>
#include <methan/private/framework/threading/signal.hpp>
#include <limits>
#include <thread>
#include <chrono>


using namespace std::literals;
using namespace Methan;
typedef unsigned long long ull;

void _working_thread_fibo_1_(volatile ull* output, size_t n, Signal* signal)
{
    signal->wait([](uint32_t value){ return value == 0; });

    ull result;
    ull previous = 0;
    *output = 1;
    for(size_t i = 1; i < n; ++i)
    {
        std::this_thread::sleep_for(50ms);
        result = previous + *output;
        previous = *output;
        *output = result;
    }

    signal->signal(1);

    std::this_thread::sleep_for(1000ms);

    *output = 0;
    signal->signal(0);

    std::this_thread::sleep_for(1000ms);
    signal->signal(2);
}

TEST_CASE("Signal is working onto the same computer (without using observer)", "[threading]") {
    INFO("Notice that this test uses multiple thread in order to ensure that multithread work");
    Context context = ContextBuilder()
        .add_logger_stdout(ELogLevel::Debug)
        .build();

    ull output = std::numeric_limits<ull>::max();
    Signal* signal = new Signal(context, generate_uuid(context));

    std::thread thread1(_working_thread_fibo_1_, &output, 20, signal);
    signal->wait([](uint32_t n) { return n != 0; });

    REQUIRE(output == 6765); // Normally the thread has terminate !!

    std::cv_status status = signal->wait_for(2000ms, [](uint32_t n) { return n == 0; });
    REQUIRE(output == 0);
    REQUIRE(status == std::cv_status::no_timeout);

    status = signal->wait_for(500ms, [](uint32_t n) { return n == 2; });
    REQUIRE(status == std::cv_status::timeout);
        thread1.join();
    delete signal;

    Methan::free(context);
}
