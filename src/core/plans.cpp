//
// Created by bryan on 12/11/22.
//

#include "plans.h"


void plans::plan_DUAVC(const ompl::base::StateSpacePtr& space, json_parser::json_obj& json_obj,
                       std::vector<pathfinder_output>& output_objects, const std::vector<simple_square*>* squares)
{
    // Scoped state
    ompl::base::ScopedState<> start_scoped(space), goal_scoped(space);

    // Convert general coordinate state to scoped state
    start_scoped[0] = json_obj.start[0];
    start_scoped[1] = json_obj.start[1];
    start_scoped[2] = json_obj.start[2];
    goal_scoped[0] = json_obj.goal[0];
    goal_scoped[1] = json_obj.goal[1];
    goal_scoped[2] = json_obj.goal[2];

    // Declare bounds
    ompl::base::RealVectorBounds bounds(2);

    // Set the bounds: x-axis
    bounds.setLow(0, json_obj.lon_left);
    bounds.setHigh(0, json_obj.lon_right);

    // Set the bounds: y-axis
    bounds.setLow(1, json_obj.lat_lower);
    bounds.setHigh(1, json_obj.lat_upper);

    // Set the bounds
    space->as<ompl::base::SE2StateSpace>()->setBounds(bounds);

    // Declare a SS (Simple Setup) class
    ompl::geometric::SimpleSetup ss(space);

    // Set state validity checking for this space
    ompl::base::SpaceInformation *si = ss.getSpaceInformation().get();
    ompl::base::SpaceInformationPtr siPtr = ss.getSpaceInformation();

    // Set the State Validity Checker
    ompl::base::ProblemDefinition pdef  (siPtr);

    auto isStateValid = state_validity_checker::isStateValidSquares;
    ss.setStateValidityChecker([isStateValid, si, squares](const ompl::base::State *state)
                               {
                                   return isStateValid(si, state, squares);
                               });

    // Minimum radius for the turns
    const double min_radius = 1;

    // Set the minimum radius in the 'Extended' class
    ss.getStateSpace()->as<Ampli_DubinsStateStateSpace>()->setRadius(min_radius);

    // Set the motion validator
    si->setMotionValidator(std::make_shared<DubinsMotionValidator_Squares>(si, squares));

    // Set the 'start_scoped' and 'goal_scoped' poses
    ss.setStartAndGoalStates(start_scoped, goal_scoped);

    // This call is optional, but we put it in to get more output information
    ss.getSpaceInformation()->setStateValidityCheckingResolution(0.0005);

    // Set the planner to be used
    ompl::base::PlannerPtr planner(new ompl::geometric::RRT(siPtr));

    // Set the Goal Bias of the planner
    planner->as<ompl::geometric::RRT>()->setGoalBias(0.05);

    // Set the range to be used
    planner->as<ompl::geometric::RRT>()->setRange(15);

    // Set the planner chosen and defined above
    ss.setPlanner(planner);

    // Setup
    ss.setup();

    // Print information
    ss.print();

    // Attempt to solve the problem within 0.05 seconds of planning time
    ompl::base::PlannerStatus solved = ss.solve(0.05);

    // Check if solved
    if (solved)
    {
        // Info: notice the user that we've found a valid solution
        std::cout << "Found solution!" << std::endl;

        // Info: we are going to print some files
        std::cout << "Some files are going to be written!" << std::endl;

        // Dump solution into some files
        plans::dump_solution(output_objects, ss);
    }
    else
    {
        // Info: no valid solution found!
        std::cout << "No solution found!" << std::endl;
    }
}


void plans::dump_solution(std::vector<pathfinder_output> &output_objects, ompl::geometric::SimpleSetup& ss)
{
    // From the ss, get the space information pointer and the planner
    auto siPtr = ss.getSpaceInformation();
    auto planner = ss.getPlanner();

    // Simplify solution output:
    // TODO: Research what the hell the "Simplify" function does, are we loosing too much accuracy?
    ss.simplifySolution();

    // Get the solution path as a list of reals
    ompl::geometric::PathGeometric geometric_path = ss.getSolutionPath();

    // Get the planner data, so we can extract useful information such as the expanded search tree
    auto planner_data = new ompl::base::PlannerData(siPtr);

    // Get the planner data from the planner data (what are we exactly doing here?)
    planner->getPlannerData(*planner_data);

    // Create file stream
    for (auto & output_obj : output_objects)
    {
        if (output_obj.get_launch())
        {
            switch (output_obj.get_type())
            {
                case OUTPUT_TYPE::PLAN:
                {
                    plans::dump_plan(output_obj.get_path(), geometric_path);
                    break;
                }
                case OUTPUT_TYPE::GRAPH:
                {
                    plans::dump_graph(output_obj.get_path(), planner_data);
                    break;
                }
                case OUTPUT_TYPE::VERTEXES:
                {
                    plans::dump_vertex(output_obj.get_path(), planner_data);
                    break;
                }
                default:
                {
                    break;
                }
            }
        }
    }
}

void plans::dump_plan(const std::string& file_path, ompl::geometric::PathGeometric& geometric_path)
{
    // Create file stream
    std::ofstream plan_file_stream;
    plan_file_stream.open(file_path);

    // Interpolate the path, getting as a result 1000 points
    geometric_path.interpolate(1000);

    // Print the path as a matrix in the given stream
    geometric_path.printAsMatrix(plan_file_stream);

    // Close the stream
    plan_file_stream.close();
}

void plans::dump_graph(const std::string& file_path, ompl::base::PlannerData * planner_data)
{
    // Create the graph viz file stream
    std::ofstream graph_viz_stream;
    graph_viz_stream.open (file_path);

    // Print the graph (from the expanded tree) in the stream, it has its own format '.viz'!
    // Docu: https://ompl.kavrakilab.org/classompl_1_1base_1_1PlannerData.html
    planner_data->printGraphviz(graph_viz_stream);

    // Close the stream
    graph_viz_stream.close();
}

void plans::dump_vertex(const std::string &file_path, ompl::base::PlannerData * planner_data)
{
    // Create the file stream
    std::ofstream vertexes_vrt_stream;
    vertexes_vrt_stream.open (file_path);

    // Get the number of vertices that the tree has
    auto num_vertices = planner_data->numVertices();

    // Iterate through the number of vertices
    for (int i = 0; i < num_vertices; i++)
    {
        // Print data of this 'ith' vertex
        auto planner_data_vertex = planner_data->getVertex(i);

        // Vertex state as real vector state R2
        auto vertex_state = planner_data_vertex.getState()->as<ompl::base::DubinsStateSpace::StateType>();

        // Get x, y and yaw coordinates
        double x = vertex_state->getX();
        double y = vertex_state->getY();
        double yaw = vertex_state->getYaw();

        // Note : format
        // i = 0: goal_scoped
        // i = 1: start_scoped
        // Write in file
        vertexes_vrt_stream << std::to_string(i) << " ";
        vertexes_vrt_stream << std::to_string(x) << " ";
        vertexes_vrt_stream << std::to_string(y) << " ";
        vertexes_vrt_stream << std::to_string(yaw) << std::endl;
    }

    // Close vertexes stream
    vertexes_vrt_stream.close();
}