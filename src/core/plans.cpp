//
// Created by bryan on 12/11/22.
//

#include "plans.h"


void plans::plan_DUAVC(const ompl::base::StateSpacePtr& space, json_parser::json_obj& json_obj,
                       std::string& plan_txt_path, const std::vector<simple_square*>* squares)
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
        std::ofstream myfile;
        myfile.open (plan_txt_path);
        std::vector<double> reals;

        std::cout << "Found solution:" << std::endl;
        ss.simplifySolution();
        ompl::geometric::PathGeometric path = ss.getSolutionPath();
        path.interpolate(1000);
        path.printAsMatrix(myfile);
        myfile.close();

        auto planner_data = new ompl::base::PlannerData(siPtr);
        planner->getPlannerData(*planner_data);

        std::ofstream myfile2;
        myfile2.open ("./out/graph.viz");
        planner_data->printGraphviz(myfile2);
        myfile2.close();


        std::ofstream myfile3;
        myfile2.open ("./out/vertexes.vrt");
        auto num_vertices = planner_data->numVertices();
        for (int i = 0; i < num_vertices; i++)
        {
            // Print data of vertices
            auto planner_data_vertex = planner_data->getVertex(i);

            // Vertex state as real vector state R2
            auto vertex_state = planner_data_vertex.getState()->as<ompl::base::DubinsStateSpace::StateType>();

            // Get x, y coordinates
            double x = vertex_state->getX();
            double y = vertex_state->getY();
            double yaw = vertex_state->getYaw();

            // Note : format
            // i = 0: goal_scoped
            // i = 1: start_scoped
            // Write in file
            myfile2 << std::to_string(i) << " ";
            myfile2 << std::to_string(x) << " ";
            myfile2 << std::to_string(y) << " ";
            myfile2 << std::to_string(yaw) << std::endl;
        }

        myfile2.close();
    }
    else
        std::cout << "No solution found" << std::endl;
}