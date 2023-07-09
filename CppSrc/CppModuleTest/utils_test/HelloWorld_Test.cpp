#include <cassert>
#include <cstring>

#include "utils/HelloWorld.hpp"

int HelloWorldTest() {
    assert(!strncmp(hello_world("1234567890123456789012345678901234567890"),
                    "Hello TinyTorch(len<40)! From C++ lib.",
                    40));
    assert(!strncmp(hello_world("TinyTorchTester"),
                    "Hello TinyTorchTester! From C++ lib.",
                    40));
//    assert(!strncmp(hello_world("TinyTorchTester"),
//                    "Hello TinyTorch! From C++ lib.",
//                    40));

    return 0;
}
