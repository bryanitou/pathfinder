//
// Created by bryan on 10/4/22.
//

#include "motion_validator.h"

bool DubinsMotionValidator_Squares::is_in_Squares(const ompl::base::State *s) const
{
    // Get x & y positions:
    const auto *state = s->as<ompl::base::SE2StateSpace::StateType>();
    double x=state->getX(), y=state->getY();


    // Iterate through all the squares
    for (const auto & sq : *this->squares_)
    {
        bool s_is_in_a_square = sq->is_in_square(x, y);
        if (s_is_in_a_square)
        {
            return true;
        }
    }

    return false;
}

bool DubinsMotionValidator_Squares::checkMotion(const ompl::base::State *s1, const ompl::base::State *s2,
                                                std::pair<ompl::base::State *, double> &lastValid) const {
    /* assume motion starts in a valid configuration so s1 is valid */

    bool result = true, firstTime = true;
    ompl::base::DubinsStateSpace::DubinsPath path;
    int nd = stateSpace_->validSegmentCount(s1, s2);

    if (nd > 1)
    {
        /* temporary storage for the checked state */
        ompl::base::State *test = si_->allocState();

        for (int j = 1; j < nd; ++j)
        {
            stateSpace_->interpolate(s1, s2, (double)j / (double)nd, firstTime, path, test);
            bool si_valid = si_->isValid(test);
            double current_radius = this->stateSpace_->getRadius();
            bool is_in_square = this->is_in_Squares(test);
            if (!si_valid || is_in_square)
            {
                lastValid.second = (double)(j - 1) / (double)nd;
                if (lastValid.first != nullptr)
                    stateSpace_->interpolate(s1, s2, lastValid.second, firstTime, path, lastValid.first);
                result = false;
                break;
            }
        }
        si_->freeState(test);
    }

    if (result)
        if (!si_->isValid(s2))
        {
            lastValid.second = (double)(nd - 1) / (double)nd;
            if (lastValid.first != nullptr)
                stateSpace_->interpolate(s1, s2, lastValid.second, firstTime, path, lastValid.first);
            result = false;
        }

    if (result)
        valid_++;
    else
        invalid_++;

    return result;
}

void DubinsMotionValidator_Squares::defaultSettings()
{
    stateSpace_ = dynamic_cast<Ampli_DubinsStateStateSpace *>(si_->getStateSpace().get());
    if (stateSpace_ == nullptr)
        throw ompl::Exception("No state space for motion validator, or the cast couldn't have been done into "
                              "OMPL::BASE:: DubinsStateSpace");
}

bool DubinsMotionValidator_Squares::checkMotion(const ompl::base::State *s1, const ompl::base::State *s2) const {
    /* assume motion starts in a valid configuration so s1 is valid */
    if (!si_->isValid(s2))
        return false;

    bool result = true, firstTime = true;
    ompl::base::DubinsStateSpace::DubinsPath path;
    int nd = stateSpace_->validSegmentCount(s1, s2);

    /* initialize the queue of test positions */
    std::queue<std::pair<int, int>> pos;
    if (nd >= 2)
    {
        pos.emplace(1, nd - 1);

        /* temporary storage for the checked state */
        ompl::base::State *test = si_->allocState();

        /* repeatedly subdivide the path segment in the middle (and check the middle) */
        while (!pos.empty())
        {
            std::pair<int, int> x = pos.front();

            int mid = (x.first + x.second) / 2;
            stateSpace_->interpolate(s1, s2, (double)mid / (double)nd, firstTime, path, test);
            double current_radius = this->stateSpace_->getRadius();
            bool is_in_square = this->is_in_Squares(test);
            if (!si_->isValid(test) || is_in_square)
            {
                result = false;
                break;
            }

            pos.pop();

            if (x.first < mid)
                pos.emplace(x.first, mid - 1);
            if (x.second > mid)
                pos.emplace(mid + 1, x.second);
        }

        si_->freeState(test);
    }

    if (result)
        valid_++;
    else
        invalid_++;

    return result;
}
