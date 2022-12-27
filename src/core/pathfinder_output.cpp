//
// Created by bryan on 27/12/22.
//

#include "pathfinder_output.h"

std::string pathfinder_output::get_parent_folder()
{
    // Convert path string to 'std::filesystem::path'
    std::filesystem::path filesys_path = this->get_path();

    // Return parent folder
    return filesys_path.parent_path();
}
