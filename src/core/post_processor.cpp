//
// Created by bryan on 27/12/22.
//

#include "post_processor.h"

void post_processor::run_python_scripts(std::vector<pathfinder_output>& output_objects)
{
    // TODO: Make it available to choose the parallel work
    // Python3 to be used
    std::string cmd_call = "python3";

    // Amount of commands to be launched
    std::vector<std::string> cmd_list;

    // Reserve memory, we already now how many we have
    cmd_list.reserve(output_objects.size());

    // Locate the python script: TODO: To be an input in the future
    std::string trajectory_plotter = "./common/Tools/trajectory_plotter.py";

    // First of all, we will build all the cmd calls, after it, we will call 'em
    for(auto& path_obj : output_objects)
    {
        // First, we will get the 'type of path', this is an enum. TODO: For the while we don't use it
        auto type_of_output = path_obj.get_type();

        // Unpacking tuple
        std::string fpath_output = path_obj.get_path();
        bool should_we_launch = path_obj.get_launch();
        bool launch_parallel = path_obj.get_parallel();

        // Only build if we have to launch it
        if (should_we_launch)
        {
            // Should we launch it in parallel?
            std::string write_ampersand = launch_parallel ? " &" : "";

            // Now, we should build the command call
            std::string cmd_str = cmd_call + " " + trajectory_plotter + " " + fpath_output + write_ampersand;

            // Append to the list commands
            cmd_list.push_back(cmd_str);
        }
    }

    // Auto-launch command to post-process result files
    if (std::system(nullptr)) // Ensure system() is available
    {
        // Iterate through the list of commands
        for (auto & cmd : cmd_list)
        {
            // Launch command
            std::system(cmd.c_str());
        }
    }
}