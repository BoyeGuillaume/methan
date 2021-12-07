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
    REQUIRE_THROWS_AS(shape.indices_of(30), Methan::Exception);
#endif

    REQUIRE(shape.indices_of(0)  == std::vector<uint32_t>{0, 0, 0});
    REQUIRE(shape.indices_of(1)  == std::vector<uint32_t>{1, 0, 0});
    REQUIRE(shape.indices_of(5)  == std::vector<uint32_t>{0, 1, 0});
    REQUIRE(shape.indices_of(15) == std::vector<uint32_t>{0, 0, 1});
    REQUIRE(shape.indices_of(21) == std::vector<uint32_t>{1, 1, 1});
    
    for(size_t i = 0; i < shape.size(); ++i)
    {
        REQUIRE(shape.offset_of(shape.indices_of(i)) == i);
    }

    SlicedTensorShape sh = SlicedTensorShape(shape, { 2, 1, 0 }, { 3, 1, 2 });
    REQUIRE(sh.rank() == 3);
    REQUIRE(sh.size() == 6);
    REQUIRE(sh.parent_size() == shape.size());

#ifdef METHAN_EXPAND_ASSERTION
    REQUIRE_THROWS_AS(sh.absolute_offset_of({3, 0, 0}), Methan::Exception);
    REQUIRE_THROWS_AS(sh.absolute_offset_of({2, 1, 0}), Methan::Exception);
    REQUIRE_THROWS_AS(sh.absolute_offset_of({0, 0, 2}), Methan::Exception);
#endif

    REQUIRE(sh.absolute_offset_of({0, 0, 0}) == shape.offset_of({ 2, 1, 0 }));
    REQUIRE(sh.absolute_offset_of({1, 0, 1}) == shape.offset_of({ 3, 1, 1 }));
    REQUIRE(sh.absolute_offset_of({2, 0, 1}) == shape.offset_of({ 4, 1, 1 }));
    REQUIRE(sh.absolute_indices_of(7) == std::vector<uint32_t>{0, 0, 0});

    REQUIRE(sh.parent_offset_of({ 2, 1, 0 }) == 0);
    REQUIRE(sh.parent_offset_of({ 4, 1, 1 }) == 5);
    REQUIRE(sh.parent_offset_of({ 4, 1, 0 }) == 2);
    REQUIRE(sh.parent_offset_of({ 2, 1, 1 }) == 3);
    REQUIRE(sh.parent_indices_of(0) == std::vector<uint32_t>{ 2, 1, 0 });
    REQUIRE(sh.parent_indices_of(1) == std::vector<uint32_t>{ 3, 1, 0 });
    REQUIRE(sh.parent_indices_of(2) == std::vector<uint32_t>{ 4, 1, 0 });

    for(size_t i = 0; i < shape.size(); ++i)
    {
        const auto vec = shape.indices_of(i);
        if(vec[1] != 1 || vec[0] < 2) {
#ifdef METHAN_EXPAND_ASSERTION
            REQUIRE_THROWS_AS(sh.absolute_indices_of(i), Methan::Exception);
#endif
        }
        else {
            REQUIRE(sh.absolute_offset_of(sh.absolute_indices_of(i)) == i);
        }
    }
}