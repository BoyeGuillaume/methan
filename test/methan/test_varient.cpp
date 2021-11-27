#include <catch2/catch_test_macros.hpp>
#include <methan/core/except.hpp>

#include <functional>

#include <methan/core/configuration.hpp>
#ifndef METHAN_DEBUG
#define METHAN_DEBUG
#endif
#include <methan/utility/varient.hpp>

class Foo
{
public:
    Foo(std::function<void()> destructor, std::function<void()> constructor, std::function<void()> copy)
    : destructor(destructor),
    constructor(constructor),
    copy(copy)
    {
        constructor();
    }

    Foo(const Foo& other)
    : destructor(other.destructor),
    constructor(other.constructor),
    copy(other.copy),
    data(other.data)
    {
        other.copy();
    }

    ~Foo()
    {
        destructor();
    }

    size_t data;

private:
    std::function<void()> destructor;
    std::function<void()> constructor;
    std::function<void()> copy;
};

TEST_CASE("Varient work on non-const pointer", "[class]")
{
    int x = 90;
    int* ptr_to_x = &x;

    Methan::Varient varient(ptr_to_x);
    REQUIRE(varient.is<int*>());
    REQUIRE(varient.is<const int*>());
    REQUIRE(!varient.is_empty());
    REQUIRE(varient.type_id() == typeid(int*).hash_code());
    REQUIRE(varient.get<int*>() == &x);
    REQUIRE(varient.get<const int*>() == &x);
    REQUIRE_THROWS_AS(varient.get<float*>(), Methan::Exception);
    REQUIRE_THROWS_AS(varient.get<const float*>(), Methan::Exception);
    REQUIRE_THROWS_AS(varient.get<int>(), Methan::Exception);
}

TEST_CASE("Varient work on const pointer", "[class]")
{
    const char* data = "hello, world";

    Methan::Varient varient(data);
    REQUIRE(varient.is<const char*>());
    REQUIRE(!varient.is<char*>());
    REQUIRE(!varient.is_empty());
    REQUIRE(varient.type_id() == typeid(const char*).hash_code());
    REQUIRE(varient.get<const char*>() == data);
    REQUIRE_THROWS_AS(varient.get<const float*>(), Methan::Exception);
    REQUIRE_THROWS_AS(varient.get<const char>(), Methan::Exception);
    REQUIRE_THROWS_AS(varient.get<char*>(), Methan::Exception);
}

TEST_CASE("Varient work on reference", "[class]")
{
    size_t living = 0;
    size_t copy = 0;

    {
        Foo foo([&living]() { living--; }, [&living]() { living++; }, [&living, &copy]() { living++; copy++; });
        foo.data = 0x6C34A;
        
        Methan::Varient varient(foo);
        REQUIRE(varient.is<Foo>());
        REQUIRE(!varient.is_empty());
        REQUIRE(varient.get<Foo>().data == 0x6C34A);
        REQUIRE(varient.type_id() == typeid(Foo).hash_code());
        varient.get<Foo>().data = 5;
        REQUIRE(varient.get<Foo>().data == 5);
        REQUIRE_THROWS_AS(varient.get<Foo*>(), Methan::Exception);
        REQUIRE_THROWS_AS(varient.get<char>(), Methan::Exception);
    }

    REQUIRE(living == 0);
    REQUIRE(copy == 1);
}

TEST_CASE("Varient work non-init", "[class]")
{
    Methan::Varient varient(nullptr);
    REQUIRE(varient.is_empty());
    REQUIRE(!varient.is<void>());
    REQUIRE(!varient.is<void*>());
    REQUIRE(!varient.is<float>());
    REQUIRE(!varient.is<const char*>());
    REQUIRE_THROWS_AS(varient.get<int*>(), Methan::Exception);
    REQUIRE_THROWS_AS(varient.get<float*>(), Methan::Exception);
}

