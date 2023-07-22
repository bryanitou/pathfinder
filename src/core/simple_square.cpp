//
// Created by bryan on 10/4/22.
//

#include "simple_square.h"

simple_square::simple_square(double x_left, double x_right, double y_upper, double y_lower)
{
    this->set_bounds(x_left, x_right, y_upper, y_lower);
}

simple_square::simple_square(std::vector<double> bounds)
{
    this->set_bounds(bounds[0], bounds[1], bounds[2], bounds[3]);
}


void simple_square::set_bounds(double x_left, double x_right, double y_upper, double y_lower)
{
    // Set values
    this->x_left = x_left;
    this->x_right = x_right;
    this->y_upper = y_upper;
    this->y_lower = y_lower;

    // Check validity of the square
    bool valid_square = this->check_validity();

    // If not valid, exit program
    if (!valid_square)
    {
        std::cerr << "error: " << "Check square definition!" << "\n";
        exit(-1);
    }
}

std::vector<double> simple_square::get_limits() const {
    std::vector<double> limits = {this->x_left, this->x_right, this->y_upper, this->y_lower};
    return limits;
}

bool simple_square::is_in_square(double x, double y) const {

    bool in_x_bounds = x > this->x_left && x < this->x_right;
    bool in_y_bounds = y > this->y_lower && y < this->y_upper;
    return in_x_bounds && in_y_bounds;
}

bool simple_square::check_validity() const {

    bool x_valid = this->x_left < this->x_right;
    bool y_valid = this->y_lower < this->y_upper;
    bool valid_square = x_valid && y_valid;

    return valid_square;
}
