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
    VszrZoning() = default;

    /** Destructor of MeshZoning object */
    ~VszrZoning() = default;

    void setupPhase();
    void executionPhase();

private:

};

#endif