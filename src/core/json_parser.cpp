//
// Created by bryan on 11/12/22.
//

#include "json_parser.h"

json_parser::json_obj json_parser::parse_input_file(const std::string& filepath)
{
    // Auxiliary variables
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

    // Read the units of the numbers
    std::string units_str = tools_str::clean_bars(my_resource_obj["units"].as_str());
    bool deg2rad = units_str == "deg" || units_str == "degrees" || units_str == "degs";

    // Extract objects from there TODO: Add fallbacks in case they don't exist
    my_json_obj.goal = my_resource_obj["goal"].as_vector<double>();
    my_json_obj.start = my_resource_obj["start"].as_vector<double>();
    my_json_obj.operator_str = tools_str::clean_bars(my_resource_obj["operator"].as_str());

    // Charge squares_dict
    json_parser::read_squares(my_resource_obj, my_json_obj);

    // Parse the bounds
    json_parser::read_bounds(my_resource_obj, my_json_obj);

    // Change units only if required
    json_parser::update_units(my_json_obj, deg2rad);

    // Return the object
    return my_json_obj;
}

void json_parser::read_squares(RSJresource &RSJ_obj, json_parser::json_obj &output_json_obj)
{
    // To be returned is 'output_json_obj'
    // This function is dedicated to read the squares_dict
    auto squares_dict = RSJ_obj["squares_dict"].as_map<std::string>();

    // Iterate through each square
    for (auto & square : squares_dict)
    {
        // Convert from string to vector of doubles
        auto square_vector = tools_str::str2vector<double>(square.second);

        // Insert square
        output_json_obj.squares_dict.emplace(square.first, simple_square(square_vector));

        // Push back in the simple list
        output_json_obj.squares_list.emplace_back(square_vector);
    }

}

void json_parser::read_bounds(RSJresource &RSJ_obj, json_parser::json_obj &output_json_obj)
{
    // To be returned is 'output_json_obj'
    // This function is dedicated to read the bounds
    auto bounds_dict = RSJ_obj["bounds"].as_map<std::string>();

    // Now we have to access the parameters and save them
    output_json_obj.lat_upper = std::stod(bounds_dict["lat_upper"]);
    output_json_obj.lat_lower = std::stod(bounds_dict["lat_lower"]);
    output_json_obj.lon_right = std::stod(bounds_dict["lon_right"]);
    output_json_obj.lon_left  = std::stod(bounds_dict["lon_left"]);
}

void json_parser::update_units(json_parser::json_obj &output_json_obj, bool deg2rad)
{
    // If it is false, do nothing
    if (!deg2rad)
    {
        return;
    }

    // Update units of the obejct
    output_json_obj.lat_upper =  output_json_obj.lat_upper * constants::deg2rad;
    output_json_obj.lat_lower =  output_json_obj.lat_lower * constants::deg2rad;
    output_json_obj.lon_right =  output_json_obj.lon_right * constants::deg2rad;
    output_json_obj.lon_left = output_json_obj.lon_left  * constants::deg2rad;

    // Loop into the other vectors
    for (double & val : output_json_obj.goal)
    {
        val = val *  constants::deg2rad;
    }
    for (double & val : output_json_obj.start)
    {
        val = val *  constants::deg2rad;
    }
}