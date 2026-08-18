#include <iostream>
#include <stdexcept>

#include "utility/ConfigReader.hpp"
#include "setup/VszrZoning.hpp"
#include "report/VszrReport.hpp"

int main(int argc, char* argv[]) {
    ConfigReader::instance().setRuntimeConfigFilePath(argv[1]);

    VszrReport::instance().addTimePoint("start_execution", std::chrono::system_clock::now());

    VszrZoning vszr_setup_program;
    vszr_setup_program.setupPhase();
    vszr_setup_program.executionPhase();

    VszrReport::instance().addTimePoint("end_execution", std::chrono::system_clock::now());

    VszrReport::instance().exportData();

    return 0;
}