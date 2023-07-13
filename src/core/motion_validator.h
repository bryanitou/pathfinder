/**
 * Modification of the Motion validator in order to avoid squares_dict.
 */
#pragma once

// OMPL library
#include "ompl/base/MotionValidator.h"
#include "ompl/base/State.h"
#include "ompl/base/spaces/SE2StateSpace.h"
#include "ompl/base/SpaceInformation.h"

// Pathfinder libraries
#include "simple_square.h"
#include "Ampli_DubinsStateSpace.h"

// System libraries
#include <vector>
#include <utility>
#include <queue>

/**
 * Personalized motion validator
 */
class DubinsMotionValidator_Squares : public ompl::base::MotionValidator
{
public:
    DubinsMotionValidator_Squares(ompl::base::SpaceInformation *si,const std::vector<simple_square*>* squares)
    : MotionValidator(si), squares_(squares)
    {
        defaultSettings();
    }
    ~DubinsMotionValidator_Squares() override = default;

    bool is_in_Squares(const ompl::base::State *s) const;
    bool checkMotion(const ompl::base::State *s1, const ompl::base::State *s2) const override;
    bool checkMotion(const ompl::base::State *s1, const ompl::base::State *s2, std::pair<ompl::base::State *, double> &lastValid) const override;

private:
    const std::vector<simple_square*>* squares_;

private:
    Ampli_DubinsStateStateSpace *stateSpace_;
    void defaultSettings();
};
