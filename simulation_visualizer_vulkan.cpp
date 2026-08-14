#include <iostream>
#include <stdexcept>

#include "core/VszrCore.hpp"

int main() {
    try {
        VszrCore app{};
        app.init();
        app.run();
        app.clean();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}