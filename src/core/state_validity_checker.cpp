/**
 * State validity functions
 */

#include "state_validity_checker.h"

bool state_validity_checker::isStateValidSquares(const ompl::base::SpaceInformation *si, const ompl::base::State *state,
                                                 const std::vector<simple_square>* squares)
{
    // Safety check: ensure that the squares_dict is not a nullptr
    if (squares == nullptr)
    {
        return true;
    }

    // Set the State Space
    const auto *s = state->as<ompl::base::SE2StateSpace::StateType>();

    // Get the coordinates
    double x=s->getX(), y=s->getY();

    // Iterate through all the squares_dict and check bounds
    for (const auto & sq : *squares)
    {
        // Check if it is inside
        bool is_in_a_square = sq.is_in_square(x, y);

        // Assess bool
        if (is_in_a_square)
        {
            return false;
        }
    }

    return si->satisfiesBounds(s);
}