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

namespace plans
{
    void
    plan_DUAVC(const ompl::base::StateSpacePtr &space, std::string &plan_txt_path,
               const std::vector<double> &start_state,
               const std::vector<double> &goal_state, const std::vector<simple_square *> *squares);
}