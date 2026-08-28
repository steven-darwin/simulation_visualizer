/**
 * @file VszrZone.hpp
 * @author Steven Darwin
 * @version 0.0.1
 * @date Created : 2026-08-11
 * @date Last Modified : 2026-08-28
 *
 * @brief [Header] Mediator class for visualizer zoning process
 */

#ifndef VSZR_ZONE_HPP
#define VSZR_ZONE_HPP

#include "nlohmann/json.hpp"
using json = nlohmann::json;

class VszrZone {
public:

    static VszrZone& instance();

    VszrZone(const VszrZone&) = delete;
    VszrZone& operator=(const VszrZone&) = delete;

    void setup();
    void run();

private:
    VszrZone() = default;

};

#endif