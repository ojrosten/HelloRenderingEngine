#include "Source/avocet/OpenGL/Utilities/ContextResolver.hpp"
#include <iostream>

int main() {
    try {
        auto fn = glGetError;
        std::cout << "Got function pointer without exception: " << (void*)fn << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Exception thrown: " << e.what() << std::endl;
    }
    return 0;
}
