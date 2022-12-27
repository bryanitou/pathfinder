//
// Created by bryan on 27/12/22.
//

#pragma once

// System libraries
#include <string>
#include <vector>

// Project libraries
#include "pathfinder_output.h"

namespace post_processor
{

    /**
     * Run the python scripts
     * @param output_objects
     */
    void run_python_scripts(std::vector<pathfinder_output> &output_objects);
};

