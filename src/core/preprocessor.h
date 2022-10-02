/**
 * This header contains all the functions declarations for the scenario setting
 */
#pragma once
#include "ompl/base/State.h"

class preprocessor
{
public:
    preprocessor();

    ~preprocessor() = default;

private:
    bool set{false};
};