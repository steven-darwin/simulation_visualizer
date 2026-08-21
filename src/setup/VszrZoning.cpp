/**
 * @file VszrZoning.cpp
 * @author Steven Darwin
 * @version 0.0.1
 * @date Created : 2026-08-11
 * @date Last Modified : 2026-08-11
 *
 * @brief [Implementation] Mediator class for visualizer zoning process
 */

#include <string>
#include <vector>
#include <fstream>
#include <memory>   
#include <unordered_set>
#include <array>
#include <iostream>

#include "nlohmann/json.hpp"
using json = nlohmann::json;

#include "input-output/InputHDF5Adapter.hpp"
#include "input-output/OutputXDMFAdapter.hpp"
#include "geometry-topology/GeometryTopology.hpp"
#include "utility/ConfigReader.hpp"

#include "setup/VszrZoning.hpp"
#include "report/VszrReport.hpp"

void VszrZoning::setupPhase() {
    std::string zone_json_file_path =
        ConfigReader::instance().getRuntimeConfigValue("scmp", "staging_directory_path") +
        "/" +
        ConfigReader::instance().getRuntimeConfigValue("scmp", "file_name_prefix") +
        "_" +
        "zone.json";

    char* zone_buffer = new char[zone_json_file_path.length() + 1];
    std::memcpy(zone_buffer, zone_json_file_path.c_str(), zone_json_file_path.length());
    zone_buffer[zone_json_file_path.length()] = '\0';

    std::ifstream raw_zone(zone_buffer);
    json full_zone_data = json::parse(raw_zone);
    std::vector<json> parsed_zone = full_zone_data.at("zone").get<std::vector<json>>();
    raw_zone.close();

    bool setup_zone = true;
    while (setup_zone) {
        std::string is_zone_being_added;

        std::cout << std::endl;
        std::cout << "Adding Zone and Parameter to Visualize? (Y/N)" << std::endl;
        std::cin >> is_zone_being_added;

        if (is_zone_being_added == "Y") {
            std::cout << "-----------------------------------------------" << std::endl;
            std::string selected_zone_option;
            std::string inputted_parameter;

            unsigned int iter = 0;
            for (auto zone_iter = parsed_zone.begin(); zone_iter != parsed_zone.end(); zone_iter++) {
                std::cout << (++iter) << ". " << zone_iter->at("name").get<std::string>() << std::endl;
            }
            std::cout << "selected zone : ";
            std::cin >> selected_zone_option;

            std::cout << "parameter : ";
            std::cin >> inputted_parameter;

            std::cout << "-----------------------------------------------" << std::endl;

            json visualization_setup = {};

            std::cout << parsed_zone[std::stoi(selected_zone_option) - 1].at("name").get<std::string>() << " visualization setup: " << std::endl;

            std::string inputted_title;
            std::cout << "title : ";
            std::cin >> inputted_title;
            visualization_setup["title"] = inputted_title;
            visualization_setup["parameter"] = inputted_parameter;

            std::cout << "-----------------------------------------------" << std::endl;

            std::vector<std::string> source_file_list;

            bool setup_visualization = true;
            std::cout << "please input source_files: (write '__stop__' to stop)" << std::endl;

            while (setup_visualization) {
                std::string inputted_file_suffix;
                std::cout << "file_suffix: ";
                std::cin >> inputted_file_suffix;

                if (inputted_file_suffix == "__stop__") {
                    break;
                }
                else {
                    source_file_list.push_back(inputted_file_suffix);
                }
            }

            visualization_setup["source_file_suffix"] = source_file_list;

            std::vector<json> visualization_setup_list;

            if (full_zone_data["zone"][std::stoi(selected_zone_option) - 1].contains("visualization")) {
                std::vector<json> existing_visualization_setup_list = full_zone_data["zone"][std::stoi(selected_zone_option) - 1].at("visualization").get<std::vector<json>>();
                for (auto setup_iter = existing_visualization_setup_list.begin(); setup_iter != existing_visualization_setup_list.end(); setup_iter++) {
                    visualization_setup_list.push_back((*setup_iter));
                }
            }

            visualization_setup_list.push_back(visualization_setup);
            full_zone_data["zone"][std::stoi(selected_zone_option) - 1]["visualization"] = visualization_setup_list;
        }
        else if (is_zone_being_added == "N") {
            break;
        }
        else {
            continue;
        }
    }

    std::ofstream modified_zone(zone_buffer);
    modified_zone << std::setw(4) << full_zone_data << std::endl;
    modified_zone.close();

    std::cout << std::endl;
}

void VszrZoning::executionPhase() {
    unsigned int progress = 0;

    VszrReport::instance().addTimePoint("file_preparation_begin", std::chrono::system_clock::now());

    std::string zone_json_file_path =
        ConfigReader::instance().getRuntimeConfigValue("scmp", "staging_directory_path") +
        "/" +
        ConfigReader::instance().getRuntimeConfigValue("scmp", "file_name_prefix") +
        "_" +
        "zone.json";

    char* zone_buffer = new char[zone_json_file_path.length() + 1];
    std::memcpy(zone_buffer, zone_json_file_path.c_str(), zone_json_file_path.length());
    zone_buffer[zone_json_file_path.length()] = '\0';

    std::ifstream raw_zone(zone_buffer);
    std::vector<json> parsed_zone = json::parse(raw_zone).at("zone").get<std::vector<json>>();

    InputHDF5Adapter::ParameterMetadata input_computational_grid_parameter = {
        "computational_grid",
        {1, 3},
        GeometryTopology::Type::VERTEX
    };

    OutputXDMFAdapter::ParameterMetadata output_computational_grid_parameter = {
        "computational_grid",
        {1, 3},
        GeometryTopology::Type::VERTEX
    };

    unsigned int visualization_zone_total = 0;
    for (auto zone_iter = parsed_zone.begin(); zone_iter != parsed_zone.end(); zone_iter++) {
        if (zone_iter->contains("visualization")) {
            visualization_zone_total++;
        }
    }

    unsigned int visualization_counter = 0;
    for (auto zone_iter = parsed_zone.begin(); zone_iter != parsed_zone.end(); zone_iter++) {
        if (!zone_iter->contains("visualization")) continue;

        std::vector<json> parameter_list = zone_iter->at("visualization").get<std::vector<json>>();
        for (auto parameter_iter = parameter_list.begin(); parameter_iter != parameter_list.end(); parameter_iter++) {
            VszrReport::instance().setVisualizerConfig(zone_iter->at("name").get<std::string>(), parameter_iter->at("parameter").get<std::string>());
            
            InputHDF5Adapter::ParameterMetadata input_temperature_parameter = {
                parameter_iter->at("parameter").get<std::string>(),
                {1},
                GeometryTopology::Type::VERTEX
            };

            OutputXDMFAdapter::ParameterMetadata output_temperature_parameter = {
                parameter_iter->at("parameter").get<std::string>(),
                {1},
                GeometryTopology::Type::VERTEX
            };
            
            unsigned int file_counter = 1;
            std::vector<std::string> source_file_list = parameter_iter->at("source_file_suffix").get<std::vector<std::string>>();
            
            for (auto source_file_iter = source_file_list.begin(); source_file_iter != source_file_list.end(); source_file_iter++) {
                InputHDF5Adapter internal_input_hdf5_adapter((*source_file_iter));
                internal_input_hdf5_adapter.addSolverParameter({ input_computational_grid_parameter, input_temperature_parameter });
                std::vector<std::shared_ptr<GeometryTopology>> recreated_neutral_topology_list = internal_input_hdf5_adapter.deserialize();

                VszrReport::instance().addTimePoint("slvr_item" + std::to_string(file_counter) + "_in", std::chrono::system_clock::now());
                VszrReport::instance().addFileSuffix("in", (*source_file_iter), "h5");

                for (auto entity_iter = recreated_neutral_topology_list.begin(); entity_iter != recreated_neutral_topology_list.end(); entity_iter++) {
                    if ((*entity_iter)->getID() == zone_iter->at("entity_id").get<std::array<uint8_t, 16>>()) {
                        std::string output_file_suffix = zone_iter->at("name").get<std::string>();
                        output_file_suffix += ".item";
                        output_file_suffix += std::to_string(file_counter);

                        OutputXDMFAdapter internal_output_xdmf_adapter(output_file_suffix);
                        internal_output_xdmf_adapter.addSolverParameter({ output_computational_grid_parameter, output_temperature_parameter });
                        internal_output_xdmf_adapter.serialize((*entity_iter));

                        VszrReport::instance().addTimePoint("vszr_item" + std::to_string(file_counter) + "_out", std::chrono::system_clock::now());
                        VszrReport::instance().addFileSuffix("out", output_file_suffix, "xmf");
                        VszrReport::instance().addFileSuffix("out", output_file_suffix, "h5");

                        VszrReport::instance().addFileMapping((*source_file_iter), output_file_suffix);

                        file_counter++;
                
                        break;
                    }
                }

                progress = 0 + (visualization_counter / visualization_zone_total * 100) + ((file_counter - 1) / static_cast<double>(source_file_list.size() / visualization_zone_total) * 100);
                std::cout << "\r" << "Processing Visualized Zone " << std::string(static_cast<size_t>(std::floor(progress / 10)), '=') << "> " << progress << "%";
            }
        }

        visualization_counter++;
    }

    VszrReport::instance().addTimePoint("file_preparation_finish", std::chrono::system_clock::now());
}