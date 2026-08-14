#include <iostream>
#include <stdexcept>

#include "setup/VszrZoning.hpp"

int main(int argc, char* argv[]) {
    VszrZoning vszr_setup_program(argv[1]);
    vszr_setup_program.setupPhase();
    vszr_setup_program.executionPhase();

    return 0;
}