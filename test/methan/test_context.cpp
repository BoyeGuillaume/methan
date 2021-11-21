#include <methan/core/context.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Construction of a context and free it (2x times) in sequential", "[context]") {
    for(int i = 0; i < 2; ++i) {
        Methan::Context context = Methan::ContextBuilder()
            .add_logger_stdout(Methan::ELogLevel::Debug)
            .build();
        Methan::free(context);
    }
}

TEST_CASE("Construction of two context in parrallel", "[context]") {
    Methan::Context context1 = Methan::ContextBuilder()
        .add_logger_stdout(Methan::ELogLevel::Debug)
        .build();
    Methan::Context context2 = Methan::ContextBuilder()
        .add_logger_stdout(Methan::ELogLevel::Debug)
        .build();
    Methan::free(context2);
    Methan::free(context1);
}
