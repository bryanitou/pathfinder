

#pragma once

// System libraries
#include <vector>
#include <ostream>
#include <iostream>

class simple_square
{
public:
    simple_square(double x_left, double x_right, double y_upper, double y_lower);
    explicit simple_square(std::vector<double> bounds);
    ~simple_square() = default;

public:
    std::vector<double> get_limits() const;

    [[nodiscard]] bool is_in_square(double x, double y) const;
private:
    [[nodiscard]] bool check_validity() const;

private:// Setters
    void set_bounds(double xl, double xr, double yu, double yd);
private:
    double y_lower{};
    double y_upper{};
    double x_left{};
    double x_right{};
};