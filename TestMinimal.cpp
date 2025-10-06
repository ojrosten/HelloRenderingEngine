#include "curlew/Window/GLFWWrappers.hpp"
#include <iostream>

int main() {
    try {
        std::cout << "Creating GLFW manager..." << std::endl;
        curlew::glfw_manager manager{};
        std::cout << "GLFW manager created successfully!" << std::endl;
        
        std::cout << "Getting rendering setup..." << std::endl;
        auto setup = manager.get_rendering_setup();
        std::cout << "Rendering setup: " << setup.version.major << "." << setup.version.minor << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cout << "Exception: " << e.what() << std::endl;
        return 1;
    }
}
