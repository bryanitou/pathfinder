

#pragma once

// System libraries
#include <vector>
#include <ostream>
#include <iostream>

class simple_square
{
public:
    simple_square(double x_left, double x_right, double y_upper, double y_lower);

    ~simple_square() = default;

public:
    std::vector<double> get_limits();

    bool is_in_square(double x, double y);
private:
    bool check_validity();
private:
    double y_lower;
    double y_upper;
    double x_left;
    double x_right;
};