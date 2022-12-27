/**
 *
 */

#include "pre_processor.h"

std::vector<pathfinder_output> pre_processor::generate_default_outputs(std::string &output_farchive)
{
    // From the archive, we should generate default outputs
    std::string plan_txt_path       = output_farchive + "/plan.txt";
    std::string vertexes_vrt_path   = output_farchive + "/vertexes.vrt";
    std::string graph_viz_path      = output_farchive + "/graph.viz";

    // Manually generate the outputs
    pathfinder_output output1(OUTPUT_TYPE::PLAN, plan_txt_path);
    pathfinder_output output2(OUTPUT_TYPE::VERTEXES, vertexes_vrt_path);
    pathfinder_output output3(OUTPUT_TYPE::GRAPH, graph_viz_path);

    // Create vector to return
    std::vector<pathfinder_output> result = {output1, output2, output3};

    // Push back them
    return result;

}