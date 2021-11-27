#include <catch2/catch_test_macros.hpp>
#include <methan/core/tensor_shape.hpp>

using namespace Methan;

TEST_CASE("TensorShape test", "[tensor]")
{
    TensorShape shape = {
        5,
        3,
        2
    };

    REQUIRE(shape.rank() == 3);
    REQUIRE(shape.size() == 30);
    REQUIRE(shape.offset_of({0, 0, 0}) == 0);
    REQUIRE(shape.offset_of({1, 0, 0}) == 1);
    REQUIRE(shape.offset_of({4, 0, 0}) == 4);
    REQUIRE(shape.offset_of({0, 1, 0}) == 5);
    REQUIRE(shape.offset_of({4, 1, 0}) == 9);
    REQUIRE(shape.offset_of({0, 1, 1}) == 20);
    REQUIRE(shape.offset_of({4, 2, 1}) == shape.size() - 1);

#ifdef METHAN_EXPAND_ASSERTION
    REQUIRE_THROWS_AS(shape.offset_of({5, 0, 0}), Methan::Exception);
    REQUIRE_THROWS_AS(shape.offset_of({0, 3, 0}), Methan::Exception);
    REQUIRE_THROWS_AS(shape.offset_of({4, 3, 1}), Methan::Exception);
    REQUIRE_THROWS_AS(shape.offset_of({4, 0, 2}), Methan::Exception);
    REQUIRE_THROWS_AS(shape.index_of(30), Methan::Exception);
#endif

    REQUIRE(shape.index_of(0)  == std::vector<uint32_t>{0, 0, 0});
    REQUIRE(shape.index_of(1)  == std::vector<uint32_t>{1, 0, 0});
    REQUIRE(shape.index_of(5)  == std::vector<uint32_t>{0, 1, 0});
    REQUIRE(shape.index_of(15) == std::vector<uint32_t>{0, 0, 1});
    REQUIRE(shape.index_of(21) == std::vector<uint32_t>{1, 1, 1});
    
    for(size_t i = 0; i < shape.size(); ++i)
    {
        REQUIRE(shape.offset_of(shape.index_of(i)) == i);
    }
}
