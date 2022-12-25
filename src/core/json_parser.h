//
// Created by bryan on 11/12/22.
//
#pragma once

#include "RSJparser.tcc"
#include "tools_str.h"

namespace json_parser
{
    struct json_obj
    {
        std::string operator_str;
        std::vector<double> goal;
        std::vector<double> start;

    };

    /**
     * Parses the input file
     * @param filepath std::string
     */
    json_parser::json_obj parse_input_file(const std::string& filepath);

};

