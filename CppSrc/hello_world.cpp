#include "hello_world.hpp"

#include <string>
#include <cstring>

const char* hello_world(const char name[]) {
    const int max_length = 40;
    static char c[40] = {'\0'};
    std::string name_str(name);
    std::string message;

    message = "Hello " + name_str;
    message += "! From C++ lib.";

    if (message.length() > max_length - 1) {
        strcpy(c, "Hello TinyTorch(len<40)! From C++ lib.");
    } else {
        strcpy(c, message.c_str());
    }

    return c;
}
