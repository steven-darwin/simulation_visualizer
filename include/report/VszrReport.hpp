/**
 * @file VszrReport.hpp
 * @author Steven Darwin
 * @version 0.0.1
 * @date Created : 2026-08-17
 * @date Last Modified : 2026-08-17
 *
 * @brief [Header] Mediator class for visualizer report creation.
 */

#ifndef VSZR_REPORT_HPP
#define VSZR_REPORT_HPP

#include <string>
#include <unordered_map>
#include <array>
#include <vector>
#include <tuple>
#include <chrono>

#include "geometry-topology/GeometryTopology.hpp"
#include "setup/VszrZoning.hpp"

class VszrReport {
public:
    static VszrReport& instance();

    VszrReport(const VszrReport&) = delete;
    VszrReport& operator=(const VszrReport&) = delete;

    void setVisualizerConfig(std::string zone, std::string parameter);
    void addTimePoint(std::string label, std::chrono::time_point<std::chrono::system_clock> timestamp);
    void addFileSuffix(std::string type, std::string file_suffix, std::string extension);
    void addFileMapping(std::string in, std::string out);

    void exportData();

private:
    /** Constructor of MeshReport object */
    VszrReport() = default;
    
    std::tuple<std::string, std::string> _visualizerConfig;
    std::vector<std::tuple<std::string, std::chrono::time_point<std::chrono::system_clock>>> _timePointList;
    std::vector<std::tuple<std::string, std::string, std::string>> _fileSuffixList;
    std::vector<std::tuple<std::string, std::string>> _fileMappingList;

    std::string paddingCenteredString(std::string text, std::string padding_char, unsigned int line_width);
};

#endif
