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
     * @param output_archive_path
     * @param squares
     */
    void plan_DUAVC(const ompl::base::StateSpacePtr &space, json_parser::json_obj& json_obj,
                    std::string &output_archive_path, const std::vector<simple_square *> *squares = nullptr);


    /**
     * Dump the solution to some files
     * @param output_archive_path
     * @param ss
     */
    void dump_solution(std::string &output_archive_path, ompl::geometric::SimpleSetup &ss);
}