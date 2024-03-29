//
// Created by bryan on 11/12/22.
//
#pragma once

#include "RSJparser.tcc"
#include "tools_str.h"
#include "constants.h"
#include "simple_square.h"
#include <filesystem>

namespace json_parser
{
    struct json_obj
    {
        std::string operator_str;
        std::vector<double> goal;
        std::vector<double> start;

        double lat_upper;
        double lat_lower;
        double lon_right;
        double lon_left;

        // Collection of squares_dict
        std::unordered_map<std::string, simple_square> squares_dict;

        // Squares list
        std::vector<simple_square> squares_list;

        // Struct in out
        std::filesystem::path out_dir;
    };

    /**
     * Parses the input file
     * @param filepath std::string
     */
    json_parser::json_obj parse_input_file(const std::string& filepath);

    /**
     * Function structured to read the bounds
     * @param RSJ_obj
     * @param output_json_obj
     */
    void read_bounds(RSJresource& RSJ_obj, json_parser::json_obj & output_json_obj);

    /**
     * Updates the units of the whole structure 'json_obj' defined above
     * @param output_json_obj
     * @param deg2rad
     */
    void update_units(json_parser::json_obj & output_json_obj, bool deg2rad);

    void read_squares(RSJresource &RSJ_obj, json_obj &output_json_obj);
};

