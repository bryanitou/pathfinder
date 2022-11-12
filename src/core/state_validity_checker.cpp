//
// Created by bryan on 12/11/22.
//

#include "state_validity_checker.h"

bool state_validity_checker::isStateValidSquares(const ompl::base::SpaceInformation *si, const ompl::base::State *state,
                                                 const std::vector<simple_square*>* squares)
{
    const auto *s = state->as<ompl::base::SE2StateSpace::StateType>();
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