#include <methan/core/lib.hpp>
#include <iostream>

DLL int test() {
    std::cout << "Hello, from the installed library" << std::endl;
    return 0;
}

DLL bool returnTrue() {
    return true;
}
