/**
 * @file VszrZoning.cpp
 * @author Steven Darwin
 * @version 0.0.1
 * @date Created : 2026-08-11
 * @date Last Modified : 2026-08-11
 *
 * @brief [Header] Mediator class for visualizer zoning process
 */

#ifndef VSZR_ZONING_HPP
#define VSZR_ZONING_HPP

#include "nlohmann/json.hpp"
using json = nlohmann::json;

class VszrZoning {
public:

    /** Constructor of VszrZoning object */
    VszrZoning();
    VszrZoning(const char* runtime_config_file_path);

    /** Destructor of MeshZoning object */
    ~VszrZoning();

    void setupPhase();
    void executionPhase();

private:
    const char* _runtimeConfigFilePath;
};

#endif