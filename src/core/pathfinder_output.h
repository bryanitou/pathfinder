//
// Created by bryan on 27/12/22.
//

# pragma once

// System libraries
#include <string>
#include <utility>
#include <filesystem>

// Project libraries
#include "enums.h"

/**
 * Container of the attributes that an output will have
 */
class pathfinder_output {

private:
    // Private attributes of the class
    OUTPUT_TYPE output_type;
    std::string fpath;
    bool launch;
    bool parallel;

public:
    /**
     * Default constructor
     * @param output_type
     * @param fpath
     * @param launch
     * @param parallel
     */
    pathfinder_output(OUTPUT_TYPE output_type, std::string& fpath, bool launch=false, bool parallel=false) :
    output_type(output_type), fpath(fpath), launch(launch), parallel(parallel) {}

    /*
     * Default destructor
     */
    ~pathfinder_output() = default;

public: // Getters

    /**
     * Get path
     * @return std::string
     */
    std::string get_path() { return this->fpath;}

    /**
     * Get type
     * @return
     */
    OUTPUT_TYPE get_type() { return this->output_type; }

    /**
     * Get launch
     * @return
     */
    [[nodiscard]] bool get_launch() const { return this->launch; }

    /**
     * Get parallel
     * @return
     */
    [[nodiscard]] bool get_parallel() const { return this->parallel; }


public: // Some other complex getters

    /**
     * Get parent folder of the output
     * @return
     */
    std::string get_parent_folder();

};
