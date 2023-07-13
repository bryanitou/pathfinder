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
    std::filesystem::path trajectory_plotter_relpath = "common/Tools/trajectory_plotter.py";
    std::string trajectory_plotter_abspath = std::filesystem::absolute(trajectory_plotter_relpath);

    // Do we have graph and vertexes?
    std::string plan_fpath{};
    std::string vertexes_fpath{};
    std::string graph_fpath{};

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
        if (OUTPUT_TYPE::PLAN == type_of_output)
        {
            if (should_we_launch)
            {
                // Should we launch it in parallel?
                std::string write_ampersand = launch_parallel ? " &" : "";

                // Now, we should build the command call
                std::string cmd_str = cmd_call;
                cmd_str += " ";
                cmd_str += trajectory_plotter_abspath;
                cmd_str += " ";
                cmd_str += fpath_output;
                cmd_str += " ";
                cmd_str += write_ampersand;

                // Append to the list commands
                cmd_list.push_back(cmd_str);
            }

            // Save plan
            plan_fpath = fpath_output;
        }

        // Check if we do have graph + vertexes
        if (OUTPUT_TYPE::VERTEXES == type_of_output && should_we_launch)
        {
            vertexes_fpath = fpath_output;

        }
        if (OUTPUT_TYPE::GRAPH == type_of_output && should_we_launch)
        {
            graph_fpath = fpath_output;
        }
    }

    // Make complex call to plot the graph + the plan
    if (!graph_fpath.empty() && !vertexes_fpath.empty())
    {
        // Now, we should build the command call
        std::string cmd_str = cmd_call + " " + trajectory_plotter_abspath;
        cmd_str += " " + plan_fpath;
        cmd_str += " " + graph_fpath;
        cmd_str += " " + vertexes_fpath;

        // Append to the list commands
        cmd_list.push_back(cmd_str);
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