
#pragma once

// OMPL library
#include "ompl/base/MotionValidator.h"
#include "ompl/base/State.h"
#include "ompl/base/spaces/SE2StateSpace.h"
#include "ompl/base/SpaceInformation.h"

// Pathfinder libraries
#include "simple_square.h"
#include "ompl/base/spaces/DubinsStateSpace.h"

// System libraries
#include <vector>
#include <utility>
#include <queue>

// Namespace
namespace base = ompl::base;

/**
 * Personalized motion validator
 */
class DubinsMotionValidator_Squares : public base::MotionValidator
{
public:
    DubinsMotionValidator_Squares(base::SpaceInformation *si,const std::vector<simple_square*>* squares)
    : MotionValidator(si), squares_(squares)
    {
        defaultSettings();
    }
    ~DubinsMotionValidator_Squares() override = default;

    bool is_in_Squares(const base::State *s) const;
    bool checkMotion(const base::State *s1, const base::State *s2) const override;
    bool checkMotion(const base::State *s1, const base::State *s2, std::pair<base::State *, double> &lastValid) const override;

private:
    const std::vector<simple_square*>* squares_;

private:
    base::DubinsStateSpace *stateSpace_;
    void defaultSettings();
};
