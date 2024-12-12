#include "first_app.h"

// std
#include <cstdlib>
#include <stdexcept>
#include <iostream>


int main() {

    vl::FirstApp app{};

    try
    {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}