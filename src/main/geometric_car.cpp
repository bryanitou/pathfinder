/*********************************************************************
* Software License Agreement (BSD License)
*
*  Copyright (c) 2010, Rice University
*  All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*   * Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*   * Redistributions in binary form must reproduce the above
*     copyright notice, this list of conditions and the following
*     disclaimer in the documentation and/or other materials provided
*     with the distribution.
*   * Neither the name of the Rice University nor the names of its
*     contributors may be used to endorse or promote products derived
*     from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
*  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
*  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
*  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
*  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
*  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*********************************************************************/

/* Author: Mark Moll */

// Third party libraries
#include <ompl/base/spaces/DubinsStateSpace.h>
#include <ompl/base/spaces/ReedsSheppStateSpace.h>
#include <ompl/base/ScopedState.h>
#include <ompl/base/MotionValidator.h>
#include <ompl/geometric/SimpleSetup.h>
#include <boost/program_options.hpp>
#include "ompl/geometric/planners/rrt/RRT.h"

// System libraries
#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>

// Pathfinder libraries
#include "simple_square.h"
#include "motion_validator.h"
#include "state_sampler.h"

// Namespacing
namespace ob = ompl::base;
namespace og = ompl::geometric;
namespace po = boost::program_options;

// The easy problem is the standard narrow passage problem: two big open
// spaces connected by a narrow passage. The hard problem is essentially
// one long narrow passage with the robot facing towards the long walls
// in both the start and goal configurations.

bool isStateValidEasy(const ob::SpaceInformation *si, const ob::State *state, const std::vector<simple_square*>* squares)
{
    const auto *s = state->as<ob::SE2StateSpace::StateType>();
    double x=s->getX(), y=s->getY();

    // Iterate through all the squares
    for (const auto & sq : *squares)
    {
        bool is_in_a_square = sq->is_in_square(x, y);
        if (is_in_a_square)
        {
            return false;
        }
    }

    return si->satisfiesBounds(s);
}

ob::StateSamplerPtr allocDubinsStateSampler_Radius(ob::StateSpace *space)
{
    return std::make_shared<DubinsStateSampler_Radius>(space);
}


void plan(const ob::StateSpacePtr& space, bool easy, std::string& plan_txt_path, const std::vector<simple_square*>* squares)
{
    ob::ScopedState<> start(space), goal(space);
    ob::RealVectorBounds bounds(2);
    bounds.setLow(0);
    if (easy)
        bounds.setHigh(18);
    else
    {
        bounds.high[0] = 6;
        bounds.high[1] = .6;
    }
    space->as<ob::SE2StateSpace>()->setBounds(bounds);

    // define a simple setup class
    og::SimpleSetup ss(space);

    // set state validity checking for this space
    ob::SpaceInformation *si = ss.getSpaceInformation().get();
    ob::SpaceInformationPtr siPtr = ss.getSpaceInformation();

    ob::ProblemDefinition pdef  (siPtr);

    auto isStateValid = isStateValidEasy;
    ss.setStateValidityChecker([isStateValid, si, squares](const ob::State *state)
                               {
                                   return isStateValid(si, state, squares);
                               });

    const double min_radius = 1; // meters
    ss.getStateSpace()->as<Ampli_DubinsStateStateSpace>()->setRadius(min_radius);
    si->setMotionValidator(std::make_shared<DubinsMotionValidator_Squares>(si, squares));

    // set the start and goal states
    start[0] = 4;
    start[1] = 5;
    start[2] = 0.;
    goal[0] = 2;
    goal[1] = 5;
    goal[2] = 0.;

    ss.setStartAndGoalStates(start, goal);

    // this call is optional, but we put it in to get more output information
    ss.getSpaceInformation()->setStateValidityCheckingResolution(0.0005);
    ob::PlannerPtr planner(new og::RRT(siPtr));
    planner->as<og::RRT>()->setGoalBias(0.05);
    planner->as<og::RRT>()->setRange(15);
    ss.setPlanner(planner);
    ss.setup();
    ss.print();

    // attempt to solve the problem within 30 seconds of planning time
    ob::PlannerStatus solved = ss.solve(0.05);

    if (solved)
    {
        std::ofstream myfile;
        myfile.open (plan_txt_path);
        std::vector<double> reals;

        std::cout << "Found solution:" << std::endl;
        ss.simplifySolution();
        og::PathGeometric path = ss.getSolutionPath();
        path.interpolate(1000);
        path.printAsMatrix(myfile);
        myfile.close();

        auto planner_data = new ob::PlannerData(siPtr);
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
            auto vertex_state = planner_data_vertex.getState()->as<ob::DubinsStateSpace::StateType>();

            // Get x, y coordinates
            double x = vertex_state->getX();
            double y = vertex_state->getY();
            double yaw = vertex_state->getYaw();

            // Note : format
            // i = 0: goal
            // i = 1: start
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

void printTrajectory(const ob::StateSpacePtr& space, const std::vector<double>& pt, const std::string& filepath)
{
    if (pt.size()!=3) throw ompl::Exception("3 arguments required for trajectory option");
    const unsigned int num_pts = 50;
    ob::ScopedState<> from(space), to(space), s(space);
    std::vector<double> reals;

    from[0] = from[1] = from[2] = 0.;

    to[0] = pt[0];
    to[1] = pt[1];
    to[2] = pt[2];

    std::ofstream myfile;
    myfile.open (filepath);

    for (unsigned int i=0; i<=num_pts; ++i)
    {
        space->interpolate(from(), to(), (double)i/num_pts, s());
        reals = s.reals();
        myfile << reals[0] << ' ' << reals[1] << ' ' << reals[2] << std::endl;
    }
    myfile.close();
}

void printDistanceGrid(const ob::StateSpacePtr& space)
{
    // print the distance for (x,y,theta) for all points in a 3D grid in SE(2)
    // over [-5,5) x [-5, 5) x [-pi,pi).
    //
    // The output should be redirected to a file, say, distance.txt. This
    // can then be read and plotted in Matlab like so:
    //     x = reshape(load('distance.txt'),200,200,200);
    //     for i=1:200,
    //         contourf(squeeze(x(i,:,:)),30);
    //         axis equal; axis tight; colorbar; pause;
    //     end;
    const unsigned int num_pts = 200;
    ob::ScopedState<> from(space), to(space);
    from[0] = from[1] = from[2] = 0.;

    for (unsigned int i=0; i<num_pts; ++i)
        for (unsigned int j=0; j<num_pts; ++j)
            for (unsigned int k=0; k<num_pts; ++k)
            {
                to[0] = 5. * (2. * (double)i/num_pts - 1.);
                to[1] = 5. * (2. * (double)j/num_pts - 1.);
                to[2] = boost::math::constants::pi<double>() * (2. * (double)k/num_pts - 1.);
                std::cout << space->distance(from(), to()) << '\n';
            }

}

void printSimpleSquares(std::vector<simple_square*>* squares)
{
    // Path of the output file
    std::filesystem::path outputpath = "./out/simple_squares.txt";
    auto parentpath = outputpath.parent_path();

    std::ofstream myfile;
    myfile.open(outputpath);

    // Check if folder exists
    if (!std::filesystem::is_directory(parentpath))
    {
        std::filesystem::create_directories(parentpath);
    }

    std::vector<double> sq_lims(4);

    for (const auto & sq : *squares)
    {
        sq_lims = sq->get_limits();

        myfile << sq_lims[0] << ' ' << sq_lims[1] << ' ' << sq_lims[2] << ' ' << sq_lims[3] << std::endl;
    }
    myfile.close();
}

int main(int argc, char* argv[])
{
    try
    {
        po::options_description desc("Options");
        desc.add_options()
                ("help", "show help message")
                ("dubins", "use Dubins state space")
                ("dubinssym", "use symmetrized Dubins state space")
                ("reedsshepp", "use Reeds-Shepp state space (default)")
                ("easyplan", "solve easy planning problem and print path")
                ("hardplan", "solve hard planning problem and print path")
                ("trajectory", po::value<std::vector<double > >()->multitoken(),
                 "print trajectory from (0,0,0) to a user-specified x, y, and theta")
                ("distance", "print distance grid")
                ("default", "will use dubins and easyplan")
                ;

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc,
                                         po::command_line_style::unix_style ^ po::command_line_style::allow_short), vm);
        po::notify(vm);

        if ((vm.count("help") != 0u) || argc==1)
        {
            std::cout << desc << "\n";
            return 1;
        }
        std::string trajectory_txt_path = "./out/trajectory.txt";
        std::string plan_txt_path = "./out/plan.txt";
        std::string vertexes_vrt_path = "./out/vertexes.vrt";
        std::string graph_viz_path = "./out/graph.viz";


        auto square1 = simple_square(2, 3, 10, 0);
        auto square2 = simple_square(6, 8, 18, 5);
        auto square3 = simple_square(12, 13, 15, 0);
        auto square4 = simple_square(14, 15, 18, 5);
        auto square5 = simple_square(3.5, 5.5, 10, 5);
        auto width = 0.2;
        auto square6 = simple_square(3-width, 3+width, 7+width, 7-width);
        auto square1ptr = &square1;
        auto square2ptr = &square2;
        auto square3ptr = &square3;
        auto square4ptr = &square4;
        auto square5ptr = &square5;
        auto square6ptr = &square6;
        // Fill vector
        std::vector<simple_square*> squares{};
        auto squares_ptr = &squares;
        // squares_ptr->push_back(square1ptr);
        // squares_ptr->push_back(square2ptr);
        squares_ptr->push_back(square6ptr);

        ob::StateSpacePtr space(std::make_shared<ob::ReedsSheppStateSpace>());

        if (vm.count("dubins") != 0u || vm.count("default") != 0u)
            space = std::make_shared<Ampli_DubinsStateStateSpace>();
        if (vm.count("dubinssym") != 0u)
            space = std::make_shared<ob::DubinsStateSpace>(1., true);
        if (vm.count("easyplan") != 0u || vm.count("default") != 0u)
            plan(space, true, plan_txt_path,squares_ptr);
        if (vm.count("trajectory") != 0u)
            printTrajectory(space, vm["trajectory"].as<std::vector<double> >(), trajectory_txt_path);
        if (vm.count("distance") != 0u)
            printDistanceGrid(space);

        // Print Simple Squares
        printSimpleSquares(squares_ptr);

        // Run python
        std::string cmd_call = "python3";
        std::string trajectory_plotter = "./common/Tools/trajectory_plotter.py";
        // Auto-launch command to post-process result files
        if (std::system(nullptr)) // Ensure system() is available
        {
            // Build command
            std::string cmd{};
            cmd = cmd_call + " " + trajectory_plotter + " " + trajectory_txt_path;
            // Launch command
            std::system(cmd.c_str());

            cmd = cmd_call + " " + trajectory_plotter + " " + plan_txt_path;
            // Launch command
            std::system(cmd.c_str());

            cmd = cmd_call + " " + trajectory_plotter + " " + plan_txt_path + " " + graph_viz_path + " " + vertexes_vrt_path + " &";
            // Launch command
            std::system(cmd.c_str());
        }
    }
    catch(std::exception& e) {
        std::cerr << "error: " << e.what() << "\n";
        return 1;
    }
    catch(...) {
        std::cerr << "Exception of unknown type!\n";
    }

    return 0;
}