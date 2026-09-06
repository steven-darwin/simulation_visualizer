 /**
 * @file VszrReport.cpp
 * @author Steven Darwin
 * @version 0.0.1
 * @date Created : 2026-08-17
 * @date Last Modified : 2026-08-17
 *
 * @brief [Implementation] Mediator class for visualizer report creation.
 */

#include <fstream>

#include "utility/ConfigReader.hpp"
#include "report/VszrReport.hpp"

VszrReport& VszrReport::instance() {
    static VszrReport singleton;
    return singleton;
}

void VszrReport::setVisualizerConfig(std::string zone, std::string parameter) {
    _visualizerConfig = { zone, parameter };
}

void VszrReport::addTimePoint(std::string label, std::chrono::time_point<std::chrono::system_clock> timestamp) {
    _timePointList.push_back({ label, timestamp });
}

void VszrReport::addFileSuffix(std::string type, std::string file_suffix, std::string extension) {
    _fileSuffixList.push_back({ type, file_suffix, extension });
}

void VszrReport::addFileMapping(std::string in, std::string out) {
    _fileMappingList.push_back({ in, out });
}

void VszrReport::exportData() {
    std::ofstream report_file(
        ConfigReader::instance().getRuntimeConfigValue("scmp", "staging_directory_path") + "/" + ConfigReader::instance().getRuntimeConfigValue("scmp", "file_name_prefix") + "_report.txt",
        std::ios::app
    );

    std::ostringstream vszr_data_in_str;

    // Header
    vszr_data_in_str << paddingString("", "=", 12, "center") << "\n";
    vszr_data_in_str << paddingString("VISUALIZER", " ", 12, "center") << "\n";
    vszr_data_in_str << paddingString("", "=", 12, "center") << "\n";
    vszr_data_in_str << "\n";

    // Configuration
    vszr_data_in_str << "> Configuration" << "\n";
    vszr_data_in_str << "\n";
    vszr_data_in_str << "visualized_zone: " << std::get<0>(_visualizerConfig) << "\n";
    vszr_data_in_str << "visualized_parameter: " << std::get<1>(_visualizerConfig) << "\n";
    vszr_data_in_str << "\n";

    // Milestone
    vszr_data_in_str << "> Milestone" << "\n";
    vszr_data_in_str << "\n";

    for (auto iter = _timePointList.begin(); iter != _timePointList.end(); iter++) {
        vszr_data_in_str << std::get<0>(*iter) << " -> ";

        const std::time_t temp_time = std::chrono::system_clock::to_time_t(std::get<1>(*iter));
        vszr_data_in_str << std::put_time(std::gmtime(&temp_time), "%F %T");
        vszr_data_in_str << "." << paddingString(std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(std::get<1>(*iter).time_since_epoch()).count() % 1000), "0", 3, "right") << "\n";
    }

    vszr_data_in_str << "\n";

    // I/O Files
    vszr_data_in_str << "> I/O Files" << "\n";
    vszr_data_in_str << "\n";

    for (auto iter = _fileSuffixList.begin(); iter != _fileSuffixList.end(); iter++) {
        vszr_data_in_str << std::get<0>(*iter) << " | ";
        vszr_data_in_str << ConfigReader::instance().getRuntimeConfigValue("scmp", "file_name_prefix");
        if (std::get<1>(*iter) != "") vszr_data_in_str << "." + std::get<1>(*iter);
        vszr_data_in_str << "." + std::get<2>(*iter) << "\n";
    }
    vszr_data_in_str << "\n";

    // File Mapping
    vszr_data_in_str << "> File Mapping" << "\n";
    vszr_data_in_str << "\n";

    vszr_data_in_str << "in -> out" << "\n";
    for (auto iter = _fileMappingList.begin(); iter != _fileMappingList.end(); iter++) {
        vszr_data_in_str << ConfigReader::instance().getRuntimeConfigValue("scmp", "file_name_prefix") << "." << std::get<0>(*iter);
        vszr_data_in_str << " -> ";
        vszr_data_in_str << ConfigReader::instance().getRuntimeConfigValue("scmp", "file_name_prefix") << "." << std::get<1>(*iter) << "\n";
    }

    vszr_data_in_str << "\n";

    report_file << vszr_data_in_str.str();
    report_file.close();
}

std::string VszrReport::paddingString(std::string text, std::string padding_char, unsigned int line_width, std::string align_direction) {
    if (text.length() >= line_width) {
        return text;
    }
    else {
        while (text.length() < line_width) {
            if (align_direction == "center") {
                text = padding_char + text + padding_char;
            }
            else if (align_direction == "right") {
                text = padding_char + text;
            }
            else if (align_direction == "left") {
                text = text + padding_char;
            }
            else {
                // do nothing
            }
        }

        return text;
    }
}