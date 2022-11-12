//
// Created by bryan on 12/11/22.
//

#include "plans.h"


void plans::plan_DUAVC(const ompl::base::StateSpacePtr& space, std::string& plan_txt_path,
                       const std::vector<double>& start_state, const std::vector<double>& goal_state,
                       const std::vector<simple_square*>* squares)
{
    // Scoped state
    ompl::base::ScopedState<> start_scoped(space), goal_scoped(space);

    // Convert general coordinate state to scoped state
    start_scoped[0] = start_scoped[0];
    start_scoped[1] = start_scoped[1];
    start_scoped[2] = start_scoped[2];
    goal_scoped[0] = goal_state[0];
    goal_scoped[1] = goal_state[1];
    goal_scoped[2] = goal_state[2];

    // Bounds
    ompl::base::RealVectorBounds bounds(2);
    bounds.setLow(0);
    bounds.setHigh(18);

    space->as<ompl::base::SE2StateSpace>()->setBounds(bounds);

    // define a simple setup class
    ompl::geometric::SimpleSetup ss(space);

    // set state validity checking for this space
    ompl::base::SpaceInformation *si = ss.getSpaceInformation().get();
    ompl::base::SpaceInformationPtr siPtr = ss.getSpaceInformation();

    ompl::base::ProblemDefinition pdef  (siPtr);

    auto isStateValid = state_validity_checker::isStateValidSquares;
    ss.setStateValidityChecker([isStateValid, si, squares](const ompl::base::State *state)
                               {
                                   return isStateValid(si, state, squares);
                               });

    // Meters
    const double min_radius = 1;
    ss.getStateSpace()->as<Ampli_DubinsStateStateSpace>()->setRadius(min_radius);
    si->setMotionValidator(std::make_shared<DubinsMotionValidator_Squares>(si, squares));

    // Set the start_scoped and goal_scoped
    ss.setStartAndGoalStates(start_scoped, goal_scoped);

    // this call is optional, but we put it in to get more output information
    ss.getSpaceInformation()->setStateValidityCheckingResolution(0.0005);
    ompl::base::PlannerPtr planner(new ompl::geometric::RRT(siPtr));
    planner->as<ompl::geometric::RRT>()->setGoalBias(0.05);
    planner->as<ompl::geometric::RRT>()->setRange(15);
    ss.setPlanner(planner);
    ss.setup();
    ss.print();

    // Attempt to solve the problem within 30 seconds of planning time
    ompl::base::PlannerStatus solved = ss.solve(0.05);

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