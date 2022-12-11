//
// Created by bryan on 11/12/22.
//

#include "json_parser.h"

json_parser::json_obj json_parser::parse_input_file(const std::string& filepath)
{
    // Auxilary variables
    std::string json_text{};
    std::string json_line{};

    // Create an empty struct to be returned
    json_parser::json_obj my_json_obj{};

    // Create handler
    std::ifstream f;

    // Open file
    f.open(filepath);

    // Read file
    if (f.is_open())
    {
        // While we reading... set values
        while (f.good())
        {
            // Read line and assign it to 'json_text' variable
            f  >> json_line;
            json_text += json_line;
        }
    }
    // TODO: Add fallbacks

    // Build parsed object
    auto my_resource_obj = RSJresource (json_text);

    // Extract objects from there
    my_json_obj.goal = my_resource_obj["goal"].as_vector<double>();
    my_json_obj.start = my_resource_obj["start"].as_vector<double>();
    my_json_obj.operator_str = handy_str::clean_bars(my_resource_obj["operator"].as_str());

    // Return the object
    return my_json_obj;
}