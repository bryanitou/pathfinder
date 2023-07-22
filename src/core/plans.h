/**
 * Namespace dedicated to the planners
 */

// System libraries
#include <fstream>

// OMPL libraries
#include <ompl/base/ScopedState.h>
#include <ompl/base/MotionValidator.h>
#include <ompl/geometric/SimpleSetup.h>
#include "ompl/geometric/planners/rrt/RRT.h"

// Project libraries
#include "simple_square.h"
#include "state_validity_checker.h"
#include "motion_validator.h"
#include "json_parser.h"
#include "pathfinder_output.h"

namespace plans
{
    /**
     * Main planner function.
     * - Set the state space
     * - Set the bounds
     * - Set the check validator
     * - Set the start/goal poses
     * @param space
     * @param json_obj
     * @param output_objects
     * @param squares
     */
    void plan_DUAVC(const ompl::base::StateSpacePtr &space, json_parser::json_obj& json_obj,
                    std::vector<pathfinder_output> &output_objects, const std::vector<simple_square *> *squares = nullptr);


    /**
     * Dump the solution to some files
     * @param output_objects
     * @param ss
     */
    void dump_solution(std::vector<pathfinder_output> &output_objects, ompl::geometric::SimpleSetup &ss);

    void dump_plan(const std::string& file_path, ompl::geometric::PathGeometric &path);

    void dump_graph(const std::string &file_path, ompl::base::PlannerData * planner_data);

    void dump_vertex(const std::string &file_path, ompl::base::PlannerData * planner_data);


    void printSimpleSquares(const std::string &file_path, const std::vector<simple_square > *squares);
}