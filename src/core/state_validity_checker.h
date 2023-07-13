/**
 * Namespace for functions of state validity checker
 */

// OMPL Stuff
#include <ompl/base/MotionValidator.h>
#include <ompl/base/spaces/SE2StateSpace.h>
#include <ompl/base/SpaceInformation.h>

// Project stuff
#include "simple_square.h"

namespace state_validity_checker {

    bool isStateValidSquares(const ompl::base::SpaceInformation *si, const ompl::base::State *state,
                             const std::vector<simple_square > *squares);
};
